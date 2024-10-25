#include "context.h"

namespace utils::MS::raw::graphics::d2d::context
	{
	com_ptr create(winrt::com_ptr<ID2D1Device> d2d_device, D2D1_DEVICE_CONTEXT_OPTIONS options)
		{
		com_ptr ret;
		winrt::check_hresult(d2d_device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, ret.put()));
		return ret;
		}
	}