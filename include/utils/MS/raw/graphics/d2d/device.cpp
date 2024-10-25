#include "device.h"

namespace utils::MS::raw::graphics::d2d::device
	{
	com_ptr create(winrt::com_ptr<ID2D1Factory1> d2d_factory, winrt::com_ptr<IDXGIDevice4> dxgi_device)
		{
		com_ptr ret;
		winrt::check_hresult(d2d_factory->CreateDevice(dxgi_device.get(), ret.put()));
		return ret;
		}
	}