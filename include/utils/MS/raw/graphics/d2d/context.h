#pragma once

#include "common.h"

#include "device.h"
#include "render_target.h"

namespace utils::MS::raw::graphics::d2d
	{
	struct context : com_wrapper<ID2D1DeviceContext>
		{
		context(d2d::device& d2d_device, D2D1_DEVICE_CONTEXT_OPTIONS options = D2D1_DEVICE_CONTEXT_OPTIONS_NONE);
		context(const d2d::hwnd_render_target::com_ptr& hwnd_render_target);
		};
	}

#ifdef utils_implementation
#include "context.cpp"
#endif