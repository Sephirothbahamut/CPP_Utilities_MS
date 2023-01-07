#pragma once

#include <optional>

#include <utils/memory.h>

#include "window.h"
#include "details/style.h"

namespace utils::win32::window
	{
	class snap_on_drag : public module
		{
		public:
			snap_on_drag(window::base& base) :
				module
					{
					base,
					[this](UINT msg, WPARAM wparam, LPARAM lparam) -> std::optional<LRESULT> { return procedure(msg, wparam, lparam); }
					}
				{
				}

			long snap_max_distance{16};

		private:
			std::optional<LRESULT> procedure(UINT msg, WPARAM wparam, LPARAM lparam)
				{
				switch (msg)
					{
					case WM_ENTERSIZEMOVE:
						get_windows();
						target = result_previous = get_base().get_window_rect();
						break;

					case WM_MOVING:
						if (true)
							{
							RECT& attempted_result_win32{*reinterpret_cast<utils::observer_ptr<RECT>>(lparam)};
							rect_t attempted_result{unpack_window_size(get_base().get_handle(), attempted_result_win32)};

							utils::math::vec2l delta{attempted_result.position() - result_previous.position()};
							target.position() += delta;

							if (const auto& result_new_opt{evaluate_move()})
								{
								const auto& result_new{result_new_opt.value()};
								attempted_result_win32 = pack_window_size(get_base().get_handle(), result_new);

								result_previous = result_new;
								return 0;
								}

							result_previous = attempted_result;
							}
						break;

					case WM_EXITSIZEMOVE:
						rects.clear();
						break;
					}
				return std::nullopt;
				}

			inline static BOOL CALLBACK enumWindowCallback(HWND hwnd, LPARAM lparam) noexcept
				{
				snap_on_drag& self{*reinterpret_cast<utils::observer_ptr<snap_on_drag>>(lparam)};
				if (self.get_base().get_handle() != hwnd && IsWindowVisible(hwnd) && GetParent(hwnd) == nullptr)
					{
					RECT rect_win32;
					if (DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rect_win32, sizeof(RECT)) == S_OK)
						{
						auto rect{win32_RECT_to_rect_t(rect_win32)};
						self.rects.push_back(rect);
						}
					else if (GetWindowRect(hwnd, &rect_win32))
						{
						auto rect{win32_RECT_to_rect_t(rect_win32)};
						self.rects.push_back(rect);
						}
					}
				return TRUE;
				}

			void get_windows() noexcept 
				{
				EnumWindows(enumWindowCallback, reinterpret_cast<LPARAM>(this)); 
				}

			std::optional<rect_t> evaluate_move()
				{
				utils::math::rect<long> closest;
				utils::math::rect<long> distance{-1, -1, -1, -1};

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

			void eval_delta(const long& target, const long& other, long& closest_distance, long& closest)
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

		private:
			std::vector<rect_t> rects;
			/// <summary>
			/// Position is updated every "step".
			/// Represents where the window would be without snapping.
			/// Size is kept constant to the beginning of the move operation.
			/// </summary>
			rect_t target;
			/// <summary>
			/// Evaluated rect the previous step.
			/// Affected by snap.
			/// </summary>
			rect_t result_previous;
		};
	}