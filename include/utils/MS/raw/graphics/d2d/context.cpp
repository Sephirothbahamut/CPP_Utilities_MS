#include "context.h"

namespace utils::MS::raw::graphics::d2d
	{
	context::context(d2d::device& d2d_device, D2D1_DEVICE_CONTEXT_OPTIONS options)
		{
		winrt::check_hresult(d2d_device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, com_ptr.put()));
		}

	context::context(const d2d::hwnd_render_target::com_ptr& hwnd_render_target)
		{
		winrt::check_hresult(hwnd_render_target->QueryInterface<ID2D1DeviceContext>(com_ptr.put()));
		}
	}