#pragma once

#include "common.h"

#include <utils/graphics/colour.h>

namespace utils::MS::raw::graphics::d2d::brush
	{
	using com_ptr = winrt::com_ptr<ID2D1SolidColorBrush>;
	com_ptr create(winrt::com_ptr<ID2D1DeviceContext> d2d_context, const utils::graphics::colour::rgba_f& colour);
	}

#ifdef utils_implementation
#include "brush.cpp"
#endif