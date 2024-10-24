#pragma once

#include "common.h"

#include "factory.h"
#include "../dxgi/device.h"

namespace utils::MS::raw::graphics::d2d
	{
	struct device : ms_wrapper<ID2D1Device>
		{
		using ms_wrapper<ID2D1Device>::ms_wrapper;
		device(ID2D1Factory1* d2d_factory, IDXGIDevice4* dxgi_device);
		};
	}

#ifdef utils_implementation
#include "device.cpp"
#endif