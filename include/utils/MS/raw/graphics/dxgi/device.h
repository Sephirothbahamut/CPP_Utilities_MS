#pragma once

#include "common.h"

#include "../d3d/device.h"

namespace utils::MS::raw::graphics::dxgi
	{
	struct device : ms_wrapper<IDXGIDevice4>
		{
		using ms_wrapper<IDXGIDevice4>::ms_wrapper;
		device(ID3D11Device* d3d_device);
		};
	}

#ifdef utils_implementation
#include "device.cpp"
#endif