#pragma once

#include "common.h"

#include <d2d1_3.h>
#include "../d3d/device.h"

struct ID3D11Device;

namespace utils::MS::raw::graphics::d2d
	{
	struct device;
	}

namespace utils::MS::raw::graphics::dxgi
	{
	struct device : com_wrapper<IDXGIDevice4>
		{
		device(winrt::com_ptr<ID3D11Device> d3d_device);
		device(d2d::device& d2d_device);
		};
	}

#ifdef utils_implementation
#include "device.cpp"
#endif