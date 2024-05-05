#include "snap_on_drag.h"

#include <optional>

#include <utils/memory.h>

#include "hwnd_wrapper.h"
#include "window.h"
#include "details/style.h"
#include "../details/cast.h"
#include "../windows.h"

namespace utils::MS::window
	{
	snap_on_drag::snap_on_drag(window::base& base, const snap_on_drag::create_info& create_info) :
		module{base},
		snap_max_distance{create_info.snap_max_distance}
		{
		}

	procedure_result snap_on_drag::procedure(UINT msg, WPARAM wparam, LPARAM lparam)
		{
		switch (msg)
			{
			case WM_ENTERSIZEMOVE:
				rects.clear();
				get_windows();
				target = result_previous = get_base().get_window_rect();
				return procedure_result::next(0);

			case WM_MOVING:
				if (true)
					{
					RECT& attempted_result_win32{*reinterpret_cast<utils::observer_ptr<RECT>>(lparam)};
					rect_t attempted_result{get_base().remove_shadow_size_from(MS::details::cast(attempted_result_win32))};

					utils::math::vec2l delta{attempted_result.position() - result_previous.position()};
					target.position() += delta;

					if (const auto& result_new_opt{evaluate_move()})
						{
						const auto& result_new{result_new_opt.value()};
						attempted_result_win32 = MS::details::cast(get_base().add_shadow_size_to(result_new));

						result_previous = result_new;
						return procedure_result::stop(0);
						}

					result_previous = attempted_result;
					}
				return procedure_result::next();

			case WM_EXITSIZEMOVE:
				rects.clear();
				return procedure_result::next(0);
			}

		return procedure_result::next();
		}


	struct lparam_pass
		{
		HWND caller_hwnd;
		std::vector<rect_t>& rects;
		};

	BOOL CALLBACK enumWindowCallback(HWND hwnd, LPARAM lparam) noexcept
		{
		lparam_pass& lparams{*reinterpret_cast<utils::observer_ptr<lparam_pass>>(lparam)};
		HWND caller_hwnd{lparams.caller_hwnd};
		auto& rects{lparams.rects};

		hwnd_wrapper wrapped{hwnd};

		bool different{caller_hwnd != wrapped.get_handle()};
		bool visible{wrapped.is_visible()};
		bool parentless{!wrapped.get_parent().is_open()};

		if (caller_hwnd != wrapped.get_handle() && wrapped.is_visible() && !wrapped.get_parent().is_open())
			{
			rects.push_back(wrapped.get_window_rect());
			}
		return TRUE;
		}

	void snap_on_drag::get_windows() noexcept
		{
		lparam_pass lparam_pass{get_base().get_handle(), rects};
		EnumWindows(enumWindowCallback, reinterpret_cast<LPARAM>(&lparam_pass)); 
		}

	std::optional<rect_t> snap_on_drag::evaluate_move()
		{
		utils::math::rect<long> closest;
		utils::math::rect<long> distance{.ll{-1}, .up{-1}, .rr{-1}, .dw{-1}};

		for (const auto& rect : rects)
			{
			eval_delta(target.ll, rect.rr, distance.ll, closest.ll);
			eval_delta(target.up, rect.dw, distance.up, closest.up);
			eval_delta(target.rr, rect.ll, distance.rr, closest.rr);
			eval_delta(target.dw, rect.up, distance.dw, closest.dw);
			}

		utils::math::rect<bool> snap
			{
			.ll{distance.ll != -1 && (distance.rr == -1 || distance.ll < distance.rr)},
			.up{distance.up != -1 && (distance.dw == -1 || distance.up < distance.dw)},
			.rr{distance.rr != -1 && (distance.ll == -1 || distance.rr < distance.ll)},
			.dw{distance.dw != -1 && (distance.up == -1 || distance.dw < distance.up)},
			};

		if (!(snap.ll || snap.up || snap.rr || snap.dw)) { return std::nullopt; }

		utils::math::rect<long> ret{target};

		if (snap.ll) { ret.x() = closest.ll; }
		if (snap.up) { ret.y() = closest.up; }
		if (snap.rr) { ret.x() = closest.rr - target.width (); }
		if (snap.dw) { ret.y() = closest.dw - target.height(); }

		return ret;
		}

	void snap_on_drag::eval_delta(const long& target, const long& other, long& closest_distance, long& closest)
		{
		long distance{std::abs(target - other)};
		if (distance <= snap_max_distance)
			{
			if (closest_distance == -1 || distance < closest_distance)
				{
				closest_distance = distance;
				closest = other;
				}
			}
		}
	}