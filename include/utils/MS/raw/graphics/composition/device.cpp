#include "device.h"

namespace utils::MS::raw::graphics::d2d::composition
	{
	device(const utils::MS::raw::graphics::dxgi::device& dxgi_device)
		{
		winrt::check_hresult(DCompositionCreateDevice(dxgi_device.get(), __uuidof(com_ptr_t), com_ptr.put_void()));
		}
	}