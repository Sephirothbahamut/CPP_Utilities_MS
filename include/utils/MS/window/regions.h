#pragma once

#include <vector>

#include <utils/math/vec2.h>
#include <utils/math/rect.h>

#include "window.h"
#include "details/style.h"
#include "../base_types.h"

namespace utils::MS::window
	{
	enum hit_type : LRESULT
		{
		resize_right      = nchittest_results::htright      ,
		resize_left       = nchittest_results::htleft       ,
		resize_up         = nchittest_results::httop        ,
		resize_down       = nchittest_results::htbottom     ,
		resize_up_right   = nchittest_results::httopright   ,
		resize_up_left    = nchittest_results::httopleft    ,
		resize_down_right = nchittest_results::htbottomright,
		resize_down_left  = nchittest_results::htbottomleft ,
		client            = nchittest_results::htclient     ,
		hole              = nchittest_results::htnowhere    ,
		same_thread_hole  = nchittest_results::httransparent, //input passthrough to windows created by the same thread
		drag              = nchittest_results::htcaption
		};

	/// <summary>
	/// Used to give a resizable inner edge to borderless windows. 
	/// If you also want to have custom regions remember to add resizable_edge before regions, otherwise regions NCHITTEST results will prevent resizable_edges from being evaluated.
	/// </summary>
	class resizable_edge : public module
		{
		public:
			struct create_info 
				{
				using module_type = resizable_edge;
				int thickness{8}; 
				};

			resizable_edge(window::base& base, const create_info& create_info = {});

			int thickness;

		protected:
			virtual procedure_result procedure(UINT msg, WPARAM wparam, LPARAM lparam) override;

			std::optional<hit_type> hit_test(utils::math::vec2i coords) const noexcept;
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
				using module_type = regions;
				hit_type default_hit_type;
				std::vector<region_data_t> regions_data;
				};

			regions(window::base& base, const create_info& create_info = {});

			hit_type default_hit_type;
			std::vector<region_data_t> regions_data;

		protected:

			virtual procedure_result procedure(UINT msg, WPARAM wparam, LPARAM lparam) override;

			hit_type hit_test(utils::math::vec2i coords);
		};
	}

#ifdef utils_implementation
#include "regions.cpp"
#endif