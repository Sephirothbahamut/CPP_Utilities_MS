#pragma once

#include "device.h"

namespace utils::MS::raw::graphics::d2d::composition
	{
	struct target : com_wrapper<IDCompositionTarget>
		{
		target(const composition::device& composition_device, HWND hwnd);
		};
	}

#ifdef utils_implementation
#include "target.cpp"
#endif