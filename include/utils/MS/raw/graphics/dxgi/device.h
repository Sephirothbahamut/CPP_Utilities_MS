#pragma once

#include "common.h"

#include "../d3d/device.h"

namespace utils::MS::raw::graphics::dxgi::device
	{
	using com_ptr = winrt::com_ptr<IDXGIDevice4>;
	com_ptr create(winrt::com_ptr<ID3D11Device> d3d_device);
	}

#ifdef utils_implementation
#include "device.cpp"
#endif