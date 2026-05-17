#include "device.h"

#include "../dxgi/device.h"

namespace utils::MS::raw::graphics::d2d
	{
	device::device(const d2d::factory& d2d_factory, const dxgi::device& dxgi_device)
		{
		winrt::check_hresult(d2d_factory->CreateDevice(dxgi_device.get(), com_ptr.put()));
		}


	dxgi::device device::get_dxgi_device() const 
		{
		winrt::com_ptr<ID2D1Device5> d2d_device_v5;
		winrt::check_hresult(com_ptr->QueryInterface<ID2D1Device5>(d2d_device_v5.put()));

		winrt::com_ptr<IDXGIDevice> dxgi_device{nullptr};
		winrt::check_hresult(d2d_device_v5->GetDxgiDevice(dxgi_device.put()));
		winrt::com_ptr<IDXGIDevice4> dxgi_device_v4;
		winrt::check_hresult(dxgi_device->QueryInterface<IDXGIDevice4>(dxgi_device_v4.put()));

		dxgi::device ret;
		ret.com_ptr = dxgi_device_v4;
		return ret;
		}
	}