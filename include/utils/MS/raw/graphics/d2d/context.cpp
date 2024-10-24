#include "context.h"

namespace utils::MS::raw::graphics::d2d
	{
	context::context(ID2D1Device* d2d_device, D2D1_DEVICE_CONTEXT_OPTIONS options) : ms_wrapper{nullptr}
		{
		utils::MS::raw::throw_if_failed(d2d_device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &ptr));
		}
	}