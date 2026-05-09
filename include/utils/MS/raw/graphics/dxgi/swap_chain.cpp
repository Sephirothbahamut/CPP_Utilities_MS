#include "swap_chain.h"

#include <utils/math/rect.h>
#include "../d2d/device.h"

namespace utils::MS::raw::graphics::dxgi
	{

	static swap_chain::com_ptr_t create(const dxgi::device& dxgi_device, HWND hwnd)
		{
		RECT client_rect{0, 0, 0, 0};
		GetClientRect(hwnd, &client_rect);
		utils::math::rect<long> rectl{client_rect.left, client_rect.top, client_rect.right, client_rect.bottom};

		winrt::com_ptr<IDXGIAdapter> dxgi_adapter;
		winrt::check_hresult(dxgi_device->GetAdapter(dxgi_adapter.put()));

		winrt::com_ptr<IDXGIFactory2> dxgi_factory;
		winrt::check_hresult(dxgi_adapter->GetParent(IID_PPV_ARGS(dxgi_factory.put())));

		DXGI_SWAP_CHAIN_DESC1 desc
			{
			.Width      {static_cast<UINT>(rectl.w())},
			.Height     {static_cast<UINT>(rectl.h())},
			.Format     {DXGI_FORMAT_B8G8R8A8_UNORM},
			.Stereo     {false},
			.SampleDesc
				{
				.Count  {1},
				.Quality{0}
				},
			.BufferUsage {DXGI_USAGE_RENDER_TARGET_OUTPUT},
			.BufferCount {2},
			.Scaling     {DXGI_SCALING_NONE},
			.SwapEffect  {DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL},
			.AlphaMode   {DXGI_ALPHA_MODE_IGNORE}, 
			.Flags       {0},
			};
		DXGI_SWAP_CHAIN_FULLSCREEN_DESC desc_fullscreen
			{
			.RefreshRate{.Numerator{1}, .Denominator{0}},
			.Scaling     {DXGI_MODE_SCALING_CENTERED},
			};
				
		swap_chain::com_ptr_t ret{nullptr};
		winrt::check_hresult(dxgi_factory->CreateSwapChainForHwnd(dxgi_device.get(), hwnd, &desc, &desc_fullscreen, nullptr, ret.put()));

		dxgi_device->SetMaximumFrameLatency(1);

		return ret;
		}

	// CreateSwapchainForHWND fails with premultiplied alpha. The error tells to use CreateSwapChainForComposition
	// if I use the following method the window is indeed glassy/transparent as expected, however nothing is rendered.
	// I assume it's related to the fact that CreateSwapChainForComposition never takes information about which window it's supposed to work on,
	// so that rendering swapchain is actually never being drawn on the window, and i'm seeing a transparent window simply because it's never drawn onto.
	// I tried to look into composition docs https://learn.microsoft.com/en-us/windows/uwp/composition/composition-native-interop
	// however there's no explanation on how to "connect" that with a dxgi swapchain.
	// The closest it gets is retrieving a d2d device context from BeginDraw
	// but at that point i'd be completely skipping over the whole swapchain deal
	static swap_chain::com_ptr_t create_composition(const dxgi::device& dxgi_device, HWND hwnd)
		{
		RECT client_rect{0, 0, 0, 0};
		GetClientRect(hwnd, &client_rect);
		utils::math::rect<long> rectl{client_rect.left, client_rect.top, client_rect.right, client_rect.bottom};

		winrt::com_ptr<IDXGIAdapter> dxgi_adapter;
		winrt::check_hresult(dxgi_device->GetAdapter(dxgi_adapter.put()));

		winrt::com_ptr<IDXGIFactory2> dxgi_factory;
		winrt::check_hresult(dxgi_adapter->GetParent(IID_PPV_ARGS(dxgi_factory.put())));

		DXGI_SWAP_CHAIN_DESC1 desc
			{
			.Width      {static_cast<UINT>(rectl.w())},
			.Height     {static_cast<UINT>(rectl.h())},
			.Format     {DXGI_FORMAT_B8G8R8A8_UNORM},
			.Stereo     {false},
			.SampleDesc
				{
				.Count  {1},
				.Quality{0}
				},
			.BufferUsage {DXGI_USAGE_RENDER_TARGET_OUTPUT},
			.BufferCount {2},
			.Scaling     {DXGI_SCALING_STRETCH},
			.SwapEffect  {DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL},
			.AlphaMode   {DXGI_ALPHA_MODE_PREMULTIPLIED},
			.Flags       {0}//DXGI_SWAP_CHAIN_FLAG_FOREGROUND_LAYER},
			};
		DXGI_SWAP_CHAIN_FULLSCREEN_DESC desc_fullscreen
			{
			.RefreshRate{.Numerator{1}, .Denominator{0}},
			.Scaling     {DXGI_MODE_SCALING_CENTERED},
			};
				
		swap_chain::com_ptr_t ret{nullptr};
		winrt::check_hresult(dxgi_factory->CreateSwapChainForComposition(dxgi_device.get(), &desc, nullptr, ret.put()));

		dxgi_device->SetMaximumFrameLatency(1);

		return ret;
		}



	swap_chain::swap_chain(const dxgi::device& dxgi_device, HWND hwnd) : com_wrapper_t{create(dxgi_device, hwnd)} {}

	//temporary flag for composition mode
	swap_chain::swap_chain(const dxgi::device& dxgi_device, HWND hwnd, nullptr_t) : com_wrapper_t{create_composition(dxgi_device, hwnd)} {}

	void swap_chain::resize(utils::math::vec2u size)
		{
		HRESULT hresult{com_ptr->ResizeBuffers(2, size.x(), size.y(), DXGI_FORMAT_B8G8R8A8_UNORM, 0)};
		if (hresult == DXGI_ERROR_DEVICE_REMOVED || hresult == DXGI_ERROR_DEVICE_RESET)
			{
			throw std::runtime_error("Device removed or reset");
			}
		else { winrt::check_hresult(hresult); }
		}




	void swap_chain::present() const
		{
		HRESULT hresult{com_ptr->Present(1, 0)};
		if (hresult == DXGI_ERROR_DEVICE_REMOVED || hresult == DXGI_ERROR_DEVICE_RESET)
			{
			throw std::runtime_error("Device removed or reset");
			}
		else { winrt::check_hresult(hresult); }
		}
	}