#include "device.h"

namespace utils::MS::raw::graphics::dxgi
	{
	device::device(ID3D11Device* d3d_device) : ms_wrapper{nullptr}
		{
		utils::MS::raw::throw_if_failed(d3d_device->QueryInterface<IDXGIDevice4>(&ptr));
		}
	}