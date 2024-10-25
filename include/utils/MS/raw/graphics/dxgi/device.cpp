#include "device.h"

namespace utils::MS::raw::graphics::dxgi::device
	{
	com_ptr create(winrt::com_ptr<ID3D11Device> d3d_device)
		{
		com_ptr ret;
		winrt::check_hresult(d3d_device->QueryInterface<IDXGIDevice4>(ret.put()));
		return ret;
		}
	}