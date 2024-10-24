#include "device.h"

namespace utils::MS::raw::graphics::d2d
	{
	device::device(ID2D1Factory1* d2d_factory, IDXGIDevice4* dxgi_device) : ms_wrapper{nullptr}
		{
		utils::MS::raw::throw_if_failed(d2d_factory->CreateDevice(dxgi_device, &ptr));
		}
	}