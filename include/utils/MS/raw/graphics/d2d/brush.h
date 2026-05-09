#pragma once

#include "common.h"
#include "device.h"

#include <utils/graphics/colour.h>

namespace utils::MS::raw::graphics::d2d::brush
	{
	using com_ptr = winrt::com_ptr<ID2D1SolidColorBrush>;
	com_ptr create(d2d::context& d2d_context, const utils::graphics::colour::rgba_f& colour);
	}

#ifdef utils_implementation
#include "brush.cpp"
#endif