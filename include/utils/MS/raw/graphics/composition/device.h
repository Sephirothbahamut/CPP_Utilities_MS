#pragma once

#include "../dxgi/device.h"

namespace utils::MS::raw::graphics::d2d::composition
	{
	struct device : com_wrapper<IDCompositionDevice>
		{
		device(const utils::MS::raw::graphics::dxgi::device& dxgi_device);
		};
	}

#ifdef utils_implementation
#include "device.cpp"
#endif