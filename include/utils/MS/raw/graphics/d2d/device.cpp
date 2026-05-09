#include "device.h"

#include "../dxgi/device.h"

namespace utils::MS::raw::graphics::d2d
	{
	device::device(winrt::com_ptr<ID2D1Factory1> d2d_factory, dxgi::device& dxgi_device)
		{
		winrt::check_hresult(d2d_factory->CreateDevice(dxgi_device.get(), com_ptr.put()));
		}
	}