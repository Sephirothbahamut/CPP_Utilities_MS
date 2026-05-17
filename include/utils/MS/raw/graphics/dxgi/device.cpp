#include "device.h"
#include <d2d1_3.h>


#include "../d3d/device.h"
#include "../d2d/device.h"

namespace utils::MS::raw::graphics::dxgi
	{
	device::device(winrt::com_ptr<ID3D11Device> d3d_device)
		{
		winrt::check_hresult(d3d_device->QueryInterface<IDXGIDevice4>(com_ptr.put()));
		}
	device::device(const d2d::device& d2d_device)
		{
		winrt::com_ptr<ID2D1Device2> d2d_device_version;
		winrt::check_hresult(d2d_device->QueryInterface<ID2D1Device2>(d2d_device_version.put()));
		winrt::com_ptr<IDXGIDevice> dxgi_device;
		winrt::check_hresult(d2d_device_version->GetDxgiDevice(dxgi_device.put()));
		winrt::check_hresult(dxgi_device->QueryInterface<IDXGIDevice4>(com_ptr.put()));
		}
	}