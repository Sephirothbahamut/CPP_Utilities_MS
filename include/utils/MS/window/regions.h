#pragma once

#include <utils/math/vec2.h>
#include <utils/math/geometry/aabb.h>

#include "window.h"
#include "details/style.h"

namespace utils::MS::window
	{
	enum hit_type : LRESULT
		{
		resize_right      = HTRIGHT ,
		resize_left       = HTLEFT  ,
		resize_up         = HTTOP   ,
		resize_down       = HTBOTTOM,
		resize_up_right   = HTTOPRIGHT ,
		resize_up_left    = HTTOPLEFT  ,
		resize_down_right = HTBOTTOMRIGHT ,
		resize_down_left  = HTBOTTOMLEFT  ,
		client            = HTCLIENT ,
		hole              = HTNOWHERE,
		drag              = HTCAPTION
		};

	/// <summary>
	/// Used to give a resizable inner edge to borderless windows. 
	/// If you also want to have custom regions remember to add resizable_edge before regions, otherwise regions NCHITTEST results will prevent resizable_edges from being evaluated.
	/// </summary>
	class resizable_edge : public module
		{
		public:
			struct create_info { int thickness{8}; };

			resizable_edge(window::base& base, create_info create_info = {}) : 
				module{base}, 
				thickness{create_info.thickness}
				{
				record_procedure([this](UINT msg, WPARAM wparam, LPARAM lparam) -> std::optional<LRESULT> { return procedure(msg, wparam, lparam); });
				}

			int thickness;

		protected:
			std::optional<LRESULT> procedure(UINT msg, WPARAM wparam, LPARAM lparam)
				{
				return msg == WM_NCHITTEST ? hit_test({GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam)}) : std::nullopt;
				}

			std::optional<hit_type> hit_test(utils::math::vec2i coords) const noexcept
				{
				const auto rect{get_base().get_window_rect()};

				if (!rect.contains(coords)) { return std::nullopt; }

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
		};

	class regions : public module
		{
		public:
			struct region_data_t
				{
				hit_type hit_type{hit_type::hole};
				rect_t rect;
				};

			struct create_info
				{
				hit_type default_hit_type;
				std::vector<region_data_t> regions_data;
				};

			regions(window::base& base, const create_info& create_info) :
				module{base},
				default_hit_type{create_info.default_hit_type}, 
				regions_data{create_info.regions_data.begin(), create_info.regions_data.end()}
				{
				record_procedure([this](UINT msg, WPARAM wparam, LPARAM lparam) -> std::optional<LRESULT> { return procedure(msg, wparam, lparam); });
				}

			hit_type default_hit_type;
			std::vector<region_data_t> regions_data;

		protected:

			std::optional<LRESULT> procedure(UINT msg, WPARAM wparam, LPARAM lparam)
				{
				return msg == WM_NCHITTEST ? std::optional<LRESULT>{hit_test({GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam)})} : std::nullopt;
				}

			hit_type hit_test(utils::math::vec2i coords)
				{
				const auto rect{get_base().get_window_rect()};

				if (!rect.contains(coords)) { return hit_type::hole; }

				coords -= rect.top_left;

				for (const auto& region_data : regions_data)
					{
					if (region_data.rect.contains(coords)) { return region_data.hit_type; }
					}

				return default_hit_type;
				}
		};
	}