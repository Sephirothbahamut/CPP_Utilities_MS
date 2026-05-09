#pragma once

#include "device.h"

namespace utils::MS::raw::graphics::d2d::composition
	{
	struct visual : com_wrapper<IDCompositionVisual>
		{
		visual(const composition::device& composition_device);
		};
	}

#ifdef utils_implementation
#include "visual.cpp"
#endif