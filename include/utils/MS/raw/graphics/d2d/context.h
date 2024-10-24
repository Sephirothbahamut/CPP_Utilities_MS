#pragma once

#include "common.h"

#include "device.h"

namespace utils::MS::raw::graphics::d2d
	{
	struct context : ms_wrapper<ID2D1DeviceContext>
		{
		using ms_wrapper<ID2D1DeviceContext>::ms_wrapper;
		context(ID2D1Device* d2d_device, D2D1_DEVICE_CONTEXT_OPTIONS options = D2D1_DEVICE_CONTEXT_OPTIONS_NONE);
		};
	}

#ifdef utils_implementation
#include "context.cpp"
#endif