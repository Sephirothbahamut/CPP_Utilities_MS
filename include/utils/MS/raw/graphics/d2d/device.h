#pragma once

#include "common.h"

#include "factory.h"
#include "../dxgi/device.h"

namespace utils::MS::raw::graphics::d2d::device
	{
	using com_ptr = winrt::com_ptr<ID2D1Device>;
	com_ptr create(winrt::com_ptr<ID2D1Factory1> d2d_factory, winrt::com_ptr<IDXGIDevice4> dxgi_device);
	}

#ifdef utils_implementation
#include "device.cpp"
#endif