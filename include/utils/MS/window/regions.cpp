#include "regions.h"

#include "details/style.h"
#include "../windows.h"

namespace utils::MS::window
	{
	resizable_edge::resizable_edge(window::base& base, const create_info& create_info) :
		module{base},
		thickness{create_info.thickness}
		{
		}
	procedure_result resizable_edge::procedure(UINT msg, WPARAM wparam, LPARAM lparam)
		{
		if (msg == WM_NCHITTEST)
			{
			if (auto ret{hit_test({GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam)})})
				{
				return procedure_result::stop(ret.value());
				}
			}

		return procedure_result::next();
		}

	std::optional<hit_type> resizable_edge::hit_test(utils::math::vec2i coords) const noexcept
		{
		const auto rect{get_base().get_window_rect()};

		if (!rect.contains(coords)) { return std::nullopt; }

		if (get_base().is_maximized()) { return std::nullopt; }

		if (thickness)
			{
			const bool hit_result_left {coords.x <= rect.ll + thickness};
			const bool hit_result_right{coords.x >= rect.rr - thickness};
			const bool hit_result_up   {coords.y <= rect.up + thickness};
			const bool hit_result_down {coords.y >= rect.dw - thickness};

			if (hit_result_up)
				{
				if (hit_result_right) { return hit_type::resize_up_right; }
				if (hit_result_left ) { return hit_type::resize_up_left ; }
				return hit_type::resize_up;
				}
			if (hit_result_down)
				{
				if (hit_result_right) { return hit_type::resize_down_right; }
				if (hit_result_left ) { return hit_type::resize_down_left ; }
				return hit_type::resize_down;
				}
				
			if (hit_result_right) { return hit_type::resize_right; }
			if (hit_result_left ) { return hit_type::resize_left ; }
			}

		return std::nullopt;
		}

	regions::regions(window::base& base, const create_info& create_info) :
		module{base},
		default_hit_type{create_info.default_hit_type}, 
		regions_data{create_info.regions_data.begin(), create_info.regions_data.end()}
		{
		}

	procedure_result regions::procedure(UINT msg, WPARAM wparam, LPARAM lparam)
				{
				if (msg == WM_NCHITTEST)
					{
					return procedure_result::next(hit_test({GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam)}));
					}
				else return procedure_result::next();
				}

	hit_type regions::hit_test(utils::math::vec2i coords)
		{
		const auto rect{get_base().get_window_rect()};

		if (!rect.contains(coords)) { return hit_type::hole; }

		coords -= rect.top_left();

		for (const auto& region_data : regions_data)
			{
			if (region_data.rect.contains(coords)) { return region_data.hit_type; }
			}

		return default_hit_type;
		}
	}