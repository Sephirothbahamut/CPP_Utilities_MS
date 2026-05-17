#pragma once

#include "common.h"

namespace utils::MS::raw::graphics::composition
	{
	struct visual : com_wrapper<IDCompositionVisual>
		{
		visual(IDCompositionDevice* composition_device);
		};
	}

#ifdef utils_implementation
#include "visual.cpp"
#endif