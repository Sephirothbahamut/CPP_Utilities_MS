#pragma once

#include "common.h"

#include <utils/graphics/colour.h>

namespace utils::MS::raw::graphics::d2d
	{
	struct brush : ms_wrapper<ID2D1SolidColorBrush>
		{
		using ms_wrapper<ID2D1SolidColorBrush>::ms_wrapper;
		brush(ID2D1DeviceContext* d2d_context, const utils::graphics::colour::rgba_f& colour);
		};
	}

#ifdef utils_implementation
#include "brush.cpp"
#endif