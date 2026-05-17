#pragma once

#include "common.h"

namespace utils::MS::raw::graphics::composition
	{
	struct target : com_wrapper<IDCompositionTarget>
		{
		target(IDCompositionDevice* composition_device, HWND hwnd);
		};
	}

#ifdef utils_implementation
#include "target.cpp"
#endif