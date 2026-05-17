#pragma once

#include "common.h"


namespace utils::MS::raw::graphics::dxgi
	{
	struct device;
	}

namespace utils::MS::raw::graphics::composition
	{
	struct device : com_wrapper<IDCompositionDevice>
		{
		device(IDXGIDevice4* dxgi_device);
		};
	}

#ifdef utils_implementation
#include "device.cpp"
#endif