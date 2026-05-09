#pragma once

#include "common.h"

#include "factory.h"

namespace utils::MS::raw::graphics::dxgi
	{
	struct device;
	};

namespace utils::MS::raw::graphics::d2d
	{
	struct device : com_wrapper<ID2D1Device>
		{
		device(winrt::com_ptr<ID2D1Factory1> d2d_factory, dxgi::device& dxgi_device);
		};
	}

#ifdef utils_implementation
#include "device.cpp"
#endif