#pragma once

#include "common.h"

#include "factory.h"

namespace utils::MS::raw::graphics::dxgi
	{
	struct device;
	}

namespace utils::MS::raw::graphics::d2d
	{
	struct device : com_wrapper<ID2D1Device>
		{
		device(const d2d::factory& d2d_factory, const dxgi::device& dxgi_device);

		dxgi::device get_dxgi_device() const;
		};
	}

#ifdef utils_implementation
#include "device.cpp"
#endif