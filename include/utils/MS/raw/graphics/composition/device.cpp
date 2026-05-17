#include "device.h"

#include "../dxgi/device.h"

namespace utils::MS::raw::graphics::composition
	{
	device::device(IDXGIDevice4* dxgi_device)
		{
		winrt::check_hresult(DCompositionCreateDevice(dxgi_device, __uuidof(com_ptr_t), com_ptr.put_void()));
		}
	}