#pragma once

#include "common.h"

#include "device.h"

namespace utils::MS::raw::graphics::d2d::context
	{
	using com_ptr = winrt::com_ptr<ID2D1DeviceContext>;
	com_ptr create(winrt::com_ptr<ID2D1Device> d2d_device, D2D1_DEVICE_CONTEXT_OPTIONS options = D2D1_DEVICE_CONTEXT_OPTIONS_NONE);
	}

#ifdef utils_implementation
#include "context.cpp"
#endif