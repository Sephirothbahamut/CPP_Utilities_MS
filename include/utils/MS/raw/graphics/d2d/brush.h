#pragma once

#include "common.h"

#include <utils/graphics/colour.h>

namespace utils::MS::raw::graphics::d2d
	{
	struct brush_solid : public com_wrapper<ID2D1SolidColorBrush>
		{
		brush_solid(ID2D1DeviceContext* d2d_context, utils::graphics::colour::rgba_f rgba_f);
		};

	struct brush_gradient : public com_wrapper<ID2D1LinearGradientBrush>
		{
		struct create_info_simplified
			{
			utils::math::vec2f start_coords;
			utils::math::vec2f end_coords;
			std::vector<utils::graphics::colour::rgba_f> colours;
			};
		struct create_info
			{
			struct gradient_stop
				{
				utils::graphics::colour::rgba_f colour;
				float percent;
				};
			utils::math::vec2f start_coords;
			utils::math::vec2f end_coords;
			std::vector<gradient_stop> gradient_stops;
			};
		brush_gradient(ID2D1DeviceContext* d2d_context, const create_info           & create_info);
		brush_gradient(ID2D1DeviceContext* d2d_context, const create_info_simplified& create_info);
		};
	}

#ifdef utils_implementation
#include "brush.cpp"
#endif