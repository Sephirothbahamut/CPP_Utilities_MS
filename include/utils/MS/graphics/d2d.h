#pragma once
#include <memory>
#include <string>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <filesystem>

#include <utils/math/vec2.h>
#include <utils/math/rect.h>
#include <utils/graphics/colour.h>
#include <utils/compilation/debug.h>
#include <utils/oop/disable_move_copy.h>

#include <dcomp.h>
#include <d2d1_3.h>
#include <d3d11_3.h>
#include <dwrite_3.h>
#include <wincodec.h>
#include <wrl/client.h>
#include <windows.ui.composition.interop.h>

#include <utils/memory.h>

#pragma comment(lib, "d2d1")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "dcomp")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "dwrite")
#pragma comment(lib, "windowscodecs")

//uses old rendertargets :(
//https://github.com/krish3402/dotnet/blob/d42a2ea76a6d50c6790bbcf88a60de73cce069a9/Samples/Win7Samples/multimedia/Direct2D/SimpleDirect2DApplication/SimpleDirect2dApplication.cpp
namespace utils::MS::graphics
	{
	namespace details
		{
		inline constexpr bool enable_debug_layer = utils::compilation::debug;

		bool succeeded(HRESULT result) { return SUCCEEDED(result); }
		bool failed   (HRESULT result) { return FAILED   (result); }

		inline std::string hr_to_string(HRESULT hr) noexcept { std::stringstream ss; ss << std::hex << hr; return ss.str(); }

		inline void throw_if_failed(HRESULT hr)
			{
			if (failed(hr))
				{
				throw std::runtime_error{"Error code : " + hr_to_string(hr)};
				}
			}

		template <typename T>
		class com_ptr;

		namespace concepts
			{
			template <typename T>
			concept com_ptr = std::convertible_to<T, details::com_ptr<typename T::interface_type>>;
			template <typename T>
			concept not_com_ptr = !com_ptr<T>;
			}

		template <typename T>
		class com_ptr : protected Microsoft::WRL::ComPtr<T>
			{
			template <typename other_T>
			friend class com_ptr;

			protected:
				using self_t = com_ptr<T>;
				using inner_t = Microsoft::WRL::ComPtr<T>;

			public:
				using interface_type = Microsoft::WRL::ComPtr<T>::InterfaceType;
				using pointer = utils::observer_ptr<interface_type>;

				using Microsoft::WRL::ComPtr<T>::ComPtr;

				template <concepts::com_ptr other_T>
				other_T as() const
					{
					other_T ret;
					throw_if_failed(Microsoft::WRL::ComPtr<T>::As(&ret));
					return ret;
					}
				template <concepts::not_com_ptr other_T>
				com_ptr<other_T> as() const { return as<com_ptr<other_T>>(); }


				const pointer  operator->() const noexcept { return inner_t::operator->(); }
				      pointer  operator->()       noexcept { return inner_t::operator->(); }
				const pointer  get       () const noexcept { return inner_t::Get(); }
				      pointer  get       ()       noexcept { return inner_t::Get(); }

				utils::observer_ptr<pointer> address_of() noexcept { return Microsoft::WRL::ComPtr<T>::GetAddressOf(); }
				auto thing_for_uuidof_functions() noexcept { return Microsoft::WRL::ComPtr<T>::operator&(); }

				bool operator==(const com_ptr<T>& other) const noexcept = default;

				void reset() noexcept { inner_t::Reset(); }
			};
		}

	namespace co
		{
		struct initializer
			{
			 initializer() { details::throw_if_failed(CoInitialize(nullptr)); } //TODO CoInitializeEx
			~initializer() { CoUninitialize(); }
			};
		}
	namespace d3d
		{
		class device : public details::com_ptr<ID3D11Device2>
			{
			public:
				using com_ptr::com_ptr;
				device() : com_ptr{create()} {}

			private:
				inline static self_t create()
					{
					details::com_ptr<ID3D11Device> base_device;

					UINT creation_flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
					if constexpr (details::enable_debug_layer)
						{
						// If the project is in a debug build, enable debugging via SDK Layers with this flag.
						creation_flags |= D3D11_CREATE_DEVICE_DEBUG;
						}

					D3D_FEATURE_LEVEL feature_level_created;

					std::array<D3D_DRIVER_TYPE, 3> attempts{D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP, D3D_DRIVER_TYPE_SOFTWARE};
					HRESULT last{S_FALSE};
					for (const auto& attempt : attempts)
						{
						last = D3D11CreateDevice
						(
							nullptr,                  // specify null to use the default adapter
							attempt,
							0,
							creation_flags,           // optionally set debug and Direct2D compatibility flags
							nullptr,                  // use the lastest feature level
							0,                        // use the lastest feature level
							D3D11_SDK_VERSION,
							base_device.address_of(), // returns the Direct3D device created
							&feature_level_created,   // returns feature level of device created
							nullptr
						);
						if (details::succeeded(last)) { break; }
						}
					details::throw_if_failed(last);
					return base_device.as<self_t>();
					}
			};
		}

	namespace dxgi
		{
		struct device : details::com_ptr<IDXGIDevice3>
			{
			using com_ptr::com_ptr;
			device(const d3d::device& d3d_device) : com_ptr{[&d3d_device]
				{
				return d3d_device.as<interface_type>();
				}()} {}
			};
		}

	namespace dxgi
		{
		class swap_chain : public details::com_ptr<IDXGISwapChain1>
			{
			public:
				swap_chain(const dxgi::device& dxgi_device, HWND hwnd) : com_ptr{create(dxgi_device, hwnd)} {}

				//temporary flag for testing purposes
				swap_chain(const dxgi::device& dxgi_device, HWND hwnd, nullptr_t) : com_ptr{create_composition(dxgi_device, hwnd)} {}

				void resize(utils::math::vec2u size)
					{
					HRESULT hresult{get()->ResizeBuffers(2, size.x, size.y, DXGI_FORMAT_B8G8R8A8_UNORM, 0)};
					if (hresult == DXGI_ERROR_DEVICE_REMOVED || hresult == DXGI_ERROR_DEVICE_RESET)
						{
						throw std::runtime_error("Device removed or reset");
						}
					else { details::throw_if_failed(hresult); }
					}

				void present() const
					{
					HRESULT hresult{get()->Present(1, 0)};
					if (hresult == DXGI_ERROR_DEVICE_REMOVED || hresult == DXGI_ERROR_DEVICE_RESET)
						{
						throw std::runtime_error("Device removed or reset");
						}
					else { details::throw_if_failed(hresult); }
					}

			private:
				inline static self_t create(const dxgi::device& dxgi_device, HWND hwnd)
					{
					RECT client_rect{0, 0, 0, 0};
					GetClientRect(hwnd, &client_rect);
					utils::math::rect<long> rectl{.ll{client_rect.left}, .up{client_rect.top}, .rr{client_rect.right}, .dw{client_rect.bottom}};

					details::com_ptr<IDXGIAdapter> dxgi_adapter;
					details::throw_if_failed(dxgi_device->GetAdapter(dxgi_adapter.address_of()));

					details::com_ptr<IDXGIFactory2> dxgi_factory;
					details::throw_if_failed(dxgi_adapter->GetParent(IID_PPV_ARGS(dxgi_factory.address_of())));

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
				
					self_t ret{nullptr};
					details::throw_if_failed(dxgi_factory->CreateSwapChainForHwnd(dxgi_device.get(), hwnd, &desc, &desc_fullscreen, nullptr, ret.address_of()));

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
				inline static self_t create_composition(const dxgi::device& dxgi_device, HWND hwnd)
					{
					RECT client_rect{0, 0, 0, 0};
					GetClientRect(hwnd, &client_rect);
					utils::math::rect<long> rectl{.ll{client_rect.left}, .up{client_rect.top}, .rr{client_rect.right}, .dw{client_rect.bottom}};

					details::com_ptr<IDXGIAdapter> dxgi_adapter;
					details::throw_if_failed(dxgi_device->GetAdapter(dxgi_adapter.address_of()));

					details::com_ptr<IDXGIFactory2> dxgi_factory;
					details::throw_if_failed(dxgi_adapter->GetParent(IID_PPV_ARGS(dxgi_factory.address_of())));

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
				
					self_t ret{nullptr};
					details::throw_if_failed(dxgi_factory->CreateSwapChainForComposition(dxgi_device.get(), &desc, nullptr, ret.address_of()));

					dxgi_device->SetMaximumFrameLatency(1);

					return ret;
					}
			};
		}

	namespace dw
		{
		struct factory : details::com_ptr<IDWriteFactory7>
			{
			using com_ptr::com_ptr;
			factory() : com_ptr{[]
				{
				self_t ret{nullptr};
				details::throw_if_failed(DWriteCreateFactory
					(
					DWRITE_FACTORY_TYPE_SHARED,
					__uuidof(interface_type),
					reinterpret_cast<utils::observer_ptr<utils::observer_ptr<IUnknown>>>(ret.address_of())
					));
				return ret;
				}()} {}
			};

		class text_format : public details::com_ptr<IDWriteTextFormat>
			{
			public:
				enum class alignment_ver { top, center, bottom };
				enum class alignment_hor { left, center, justified, right };

				struct create_info
					{
					std::wstring name;
					float size{16.f};
					DWRITE_FONT_WEIGHT  weight {DWRITE_FONT_WEIGHT_NORMAL };
					DWRITE_FONT_STYLE   style  {DWRITE_FONT_STYLE_NORMAL  };
					DWRITE_FONT_STRETCH stretch{DWRITE_FONT_STRETCH_NORMAL};
					alignment_hor alignment_hor{alignment_hor::left};
					alignment_ver alignment_ver{alignment_ver::top};
					std::wstring locale{L"en-gb"};
					};

				using com_ptr::com_ptr;
	
				text_format(dw::factory& dw_factory, const create_info& create_info) : com_ptr{create(dw_factory, create_info)} {}
	
			private:
				inline static self_t create(dw::factory& dw_factory, const create_info& create_info)
					{
					self_t ret{nullptr};
					details::throw_if_failed(dw_factory->CreateTextFormat
						(
						create_info.name.c_str(),
						nullptr,
						create_info.weight,
						create_info.style,
						create_info.stretch,
						create_info.size,
						L"", //locale
						ret.address_of()
						));
					
					switch (create_info.alignment_hor)
						{
						case alignment_hor::left     : ret->SetTextAlignment(DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_LEADING  ); break;
						case alignment_hor::center   : ret->SetTextAlignment(DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_CENTER   ); break;
						case alignment_hor::justified: ret->SetTextAlignment(DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_JUSTIFIED); break;
						case alignment_hor::right    : ret->SetTextAlignment(DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_TRAILING ); break;
						}
					
					switch (create_info.alignment_ver)
						{
						case alignment_ver::top      : ret->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_NEAR  ); break;
						case alignment_ver::center   : ret->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_CENTER); break;
						case alignment_ver::bottom   : ret->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_FAR   ); break;
						}

					return ret;
					}
			};
		
		class text_layout : public details::com_ptr<IDWriteTextLayout>
			{
			public:
				enum class alignment_ver { top, center, bottom };
				enum class alignment_hor { left, center, justified, right };

				using com_ptr::com_ptr;
	
				text_layout(dw::factory& dw_factory, const std::wstring& string, const text_format& format, utils::math::vec2f box_size) : com_ptr{create(dw_factory, string, format, box_size)} {}
	
			private:
				inline static self_t create(dw::factory& dw_factory, const std::wstring& string, const text_format& format, utils::math::vec2f box_size)
					{
					self_t ret{nullptr};
					details::throw_if_failed(dw_factory->CreateTextLayout
						(
						string.c_str(), // The string to be laid out and formatted.
						string.size (), // The length of the string.
						format.get  (), // The text format to apply to the string (contains font information, etc).
						box_size.x,     // The width of the layout box.
						box_size.y,     // The height of the layout box.
						ret.address_of()
						));
					
					return ret;
					}
			};
		}

	namespace wic
		{
		struct imaging_factory : details::com_ptr<IWICImagingFactory2>
			{
			using com_ptr::com_ptr;
			imaging_factory() : com_ptr{[]
				{
				self_t ret{nullptr};
				details::throw_if_failed(CoCreateInstance
					(
					CLSID_WICImagingFactory2,
					nullptr,
					CLSCTX_INPROC_SERVER,
					IID_PPV_ARGS(ret.address_of())
					));
				return ret;
				}()} {}
			};

		class bitmap : public details::com_ptr<IWICBitmap>
			{
			public:
				using com_ptr::com_ptr;
				bitmap(const wic::imaging_factory& wic_imaging_factory, utils::math::vec2u size) : com_ptr{create(wic_imaging_factory, size)} {}
	
			private:
				inline static self_t create(const wic::imaging_factory& wic_imaging_factory, utils::math::vec2u size)
					{
					self_t ret{nullptr};
					details::throw_if_failed(wic_imaging_factory->CreateBitmap(size.x, size.y, GUID_WICPixelFormat32bppPBGRA, WICBitmapCacheOnLoad, ret.address_of()));
					return ret;
					}
			};
		
		class stream : public details::com_ptr<IWICStream>
			{
			public:
				using com_ptr::com_ptr;
				stream(const wic::imaging_factory& wic_imaging_factory, const std::filesystem::path& path) : com_ptr{create(wic_imaging_factory, path)} {}
	
			private:
				inline static self_t create(const wic::imaging_factory& wic_imaging_factory, const std::filesystem::path& path)
					{
					self_t ret{nullptr};
					details::throw_if_failed(wic_imaging_factory->CreateStream(ret.address_of()));
					details::throw_if_failed(ret->InitializeFromFilename(path.wstring().c_str(), GENERIC_WRITE));
					return ret;
					}
			};
		}

	namespace d2d
		{
		struct factory : details::com_ptr<ID2D1Factory6>
			{
			using com_ptr::com_ptr;
			factory() : com_ptr{[]
				{
				D2D1_FACTORY_OPTIONS options
					{
					.debugLevel{details::enable_debug_layer ? D2D1_DEBUG_LEVEL_INFORMATION : D2D1_DEBUG_LEVEL_NONE}
					};

				self_t ret{nullptr};

				details::throw_if_failed(D2D1CreateFactory
					(
					D2D1_FACTORY_TYPE_SINGLE_THREADED,
					options,
					ret.address_of()
					));
				return ret;
				}()} {}
			};

		class device : public details::com_ptr<ID2D1Device5>
			{
			public:
				using com_ptr::com_ptr;
				device(const d2d::factory& d2d_factory, const dxgi::device& dxgi_device) : com_ptr{create(d2d_factory, dxgi_device)} {}

				dxgi::device get_dxgi_device() const noexcept 
					{
					details::com_ptr<IDXGIDevice> ret{nullptr};
					details::throw_if_failed(get()->GetDxgiDevice(ret.address_of()));
					return ret.as<dxgi::device>();
					}

			private:
				inline static self_t create(const d2d::factory& d2d_factory, const dxgi::device& dxgi_device)
					{
					self_t ret{nullptr};
					details::throw_if_failed(d2d_factory->CreateDevice(dxgi_device.get(), ret.address_of()));
					return ret;
					}
			};

		struct render_target : details::com_ptr<ID2D1RenderTarget>
			{
			public:
				using com_ptr::com_ptr;
				render_target(const factory& factory, const wic::bitmap& bitmap) : com_ptr{[&factory, &bitmap]
					{
					self_t ret{nullptr};
					details::throw_if_failed(factory->CreateWicBitmapRenderTarget(bitmap.get(), D2D1::RenderTargetProperties(), ret.address_of()));
					return ret;
					}()} {}
			};

		struct hwnd_render_target : details::com_ptr<ID2D1HwndRenderTarget>
			{
			using com_ptr::com_ptr;
			hwnd_render_target(const factory& factory, const HWND& hwnd) : com_ptr{[&factory, &hwnd]
				{
				self_t ret{nullptr};
				D2D1_RENDER_TARGET_PROPERTIES properties
					{
					.type{D2D1_RENDER_TARGET_TYPE_DEFAULT},
					.pixelFormat
						{
						.format{DXGI_FORMAT_UNKNOWN},
						.alphaMode{D2D1_ALPHA_MODE_PREMULTIPLIED}
						}
					};

				details::throw_if_failed(factory->CreateHwndRenderTarget(properties, D2D1::HwndRenderTargetProperties(hwnd), ret.address_of()));
				return ret;
				}()} {}
			};

		class device_context : public details::com_ptr<ID2D1DeviceContext5>
			{
			public:
				using com_ptr::com_ptr;
				device_context(const d2d::device& d2d_device         ) : com_ptr{create(d2d_device)} {}
				device_context(const d2d::hwnd_render_target& hwnd_rt) : com_ptr{create(hwnd_rt   )} {}

			private:
				inline static self_t create(const d2d::device& d2d_device)
					{
					self_t ret{nullptr};
					details::throw_if_failed(d2d_device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, ret.address_of()));
					return ret;
					}

				inline static self_t create(const d2d::hwnd_render_target& hwnd_rt)
					{
					return hwnd_rt.as<interface_type>();
					}
			};

		class bitmap : public details::com_ptr<ID2D1Bitmap1>
			{
			public:
				using com_ptr::com_ptr;
				bitmap(const d2d::device_context& d2d_device_context, const dxgi::swap_chain& dxgi_swapchain) : com_ptr{create(d2d_device_context, dxgi_swapchain)} {}
				bitmap(const d2d::device_context& d2d_device_context, utils::math::vec2u size) : com_ptr{create(d2d_device_context, size)} {}
				//bitmap(const d2d::device_context& d2d_device_context, const wic::bitmap& wic_bitmap) : com_ptr{create_from_wic_bitmap  (context, wic_bitmap)} {}

			private:
				inline static self_t create(const d2d::device_context& d2d_device_context, const dxgi::swap_chain& dxgi_swapchain)
					{
					details::com_ptr<IDXGISurface2> dxgi_back_buffer;
					details::throw_if_failed(dxgi_swapchain->GetBuffer(0, IID_PPV_ARGS(dxgi_back_buffer.address_of())));

					D2D1_BITMAP_PROPERTIES1 properties
						{
						.pixelFormat{DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED},
						.dpiX{1},//TODO dpi stuff
						.dpiY{1},
						.bitmapOptions{D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW},
						};

					self_t ret{nullptr};
					details::throw_if_failed(d2d_device_context->CreateBitmapFromDxgiSurface(dxgi_back_buffer.get(), &properties, ret.address_of()));
					return ret;
					}

				inline static self_t create(const d2d::device_context& d2d_device_context)
					{
					D2D1_SIZE_U pixel_size{d2d_device_context->GetPixelSize()};
					return create(d2d_device_context, utils::math::vec2u{pixel_size.width, pixel_size.height});
					}
			
				inline static self_t create(const d2d::device_context& d2d_device_context, utils::math::vec2u size)
					{
					self_t ret{nullptr};
					details::throw_if_failed(d2d_device_context->CreateBitmap
						(
						D2D1_SIZE_U{.width{size.x}, .height{size.y}},
						nullptr, 
						size.x * 4,
						D2D1_BITMAP_PROPERTIES1
							{
							.pixelFormat{DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED},
							.bitmapOptions{D2D1_BITMAP_OPTIONS_TARGET},
							},
						ret.address_of()
						));
					return ret;
					}
			
				//inline static pointer create_from_wic_bitmap(const d2d::device_context& d2d_device_context, const wic::bitmap& wic_bitmap)
				//	{
				//	pointer ret{nullptr};
				//	details::throw_if_failed(d2d_device_context->CreateBitmapFromWicBitmap(wic_bitmap.get(), &ret));
				//	return ret;
				//	}
			};

		using brush = details::com_ptr<ID2D1Brush>;

		class solid_brush : public details::com_ptr<ID2D1SolidColorBrush>
			{
			public:
				using com_ptr::com_ptr;
				solid_brush(const d2d::device_context& d2d_device_context, utils::graphics::colour::rgba_f rgba_f) : com_ptr{create(d2d_device_context, rgba_f)} {}

			private:
				inline static self_t create(const d2d::device_context& d2d_device_context, utils::graphics::colour::rgba_f rgba_f)
					{
					self_t ret{nullptr};
					details::throw_if_failed(d2d_device_context->CreateSolidColorBrush
						(
						D2D1::ColorF{rgba_f.r, rgba_f.g, rgba_f.b, rgba_f.a},
						D2D1_BRUSH_PROPERTIES{.opacity{1}},
						ret.address_of()
						));
					return ret;
					}
			};
		}

	namespace wic
		{
		class bitmap_encoder : public details::com_ptr<IWICBitmapEncoder>
			{
			public:
				using com_ptr::com_ptr;
				bitmap_encoder(const wic::imaging_factory& wic_factory) : com_ptr{create(wic_factory)} {}

			private:
				inline static self_t create(const wic::imaging_factory& wic_factory)
					{
					self_t ret{nullptr};
					details::throw_if_failed(wic_factory->CreateEncoder(GUID_ContainerFormatPng, nullptr, ret.address_of()));
					return ret;
					}

			public:
				class frame : public details::com_ptr<IWICBitmapFrameEncode>
					{
					public:
						frame(const bitmap_encoder& encoder) : com_ptr{create(encoder)} {}

					private:
						inline static self_t create(const bitmap_encoder& encoder)
							{
							self_t ret{nullptr};
							details::throw_if_failed(encoder->CreateNewFrame(ret.address_of(), nullptr));
							return ret;
							}
				};
			};
	
		class image_encoder : public details::com_ptr<IWICImageEncoder>
			{
			public:
				using com_ptr::com_ptr;
				image_encoder(const wic::imaging_factory& wic_factory, const d2d::device& d2d_device ) : com_ptr{create(wic_factory, d2d_device )} {}

			private:
				inline static self_t create(const wic::imaging_factory& wic_factory, const d2d::device& d2d_device)
					{
					self_t ret{nullptr};
					details::throw_if_failed(wic_factory->CreateImageEncoder(d2d_device.get(), ret.address_of()));
					return ret;
					}
			};

		void save_to_file(const wic::imaging_factory& wic_factory, const d2d::device& d2d_device, const d2d::bitmap& d2d_bitmap, const std::filesystem::path& path)
			{// https://github.com/uri247/Win81App/blob/master/Direct2D%20save%20to%20image%20file%20sample/C%2B%2B/SaveAsImageFileSample.cpp

			wic::stream stream{wic_factory, path};
			wic::bitmap_encoder bitmap_encoder{wic_factory};
			details::throw_if_failed(bitmap_encoder->Initialize(stream.get(), WICBitmapEncoderNoCache));

			wic::bitmap_encoder::frame frame_encode{bitmap_encoder};
			details::throw_if_failed(frame_encode->Initialize(nullptr));

			wic::image_encoder image_encoder{wic_factory, d2d_device};

			details::throw_if_failed(image_encoder->WriteFrame(d2d_bitmap.get(), frame_encode.get(), nullptr));
			details::throw_if_failed(frame_encode->Commit());
			details::throw_if_failed(bitmap_encoder->Commit());
			details::throw_if_failed(stream->Commit(STGC_DEFAULT));
			}
		}

	namespace composition
		{
		struct device : details::com_ptr<IDCompositionDevice>
			{
			using com_ptr::com_ptr;
			device(const dxgi::device& dxgi_device) : com_ptr{[&dxgi_device]
				{
				self_t ret{nullptr};
				details::throw_if_failed(DCompositionCreateDevice(dxgi_device.get(), __uuidof(interface_type), ret.thing_for_uuidof_functions()));
				return ret;
				}()} {}
			};
		struct target : details::com_ptr<IDCompositionTarget>
			{
			using com_ptr::com_ptr;
			target(const composition::device& composition_device, HWND hwnd) : com_ptr{[&composition_device, &hwnd]
				{
				self_t ret{nullptr};
				details::throw_if_failed(composition_device->CreateTargetForHwnd(hwnd, TRUE, ret.address_of()));
				return ret;
				}()} {}
			};
		struct visual : details::com_ptr<IDCompositionVisual>
			{
			using com_ptr::com_ptr;
			visual(const composition::device& composition_device) : com_ptr{[&composition_device]
				{
				self_t ret{nullptr};
				details::throw_if_failed(composition_device->CreateVisual(ret.address_of()));
				return ret;
				}()} {}
			};
		/*struct surface : details::com_ptr<ABI::Windows::UI::Composition::ICompositionDrawingSurface>
			{
			using com_ptr::com_ptr;
			surface(const composition::device& composition_device, HWND hwnd) : com_ptr{[&composition_device, &hwnd]
				{
				self_t ret{nullptr};
				details::throw_if_failed
					(
					composition_device->CreateDrawingSurface
						(
						{128, 128},
						ABI::Windows::Graphics::DirectX::DirectXPixelFormat::DirectXPixelFormat_B8G8R8A8UIntNormalized,
						ABI::Windows::Graphics::DirectX::DirectXAlphaMode_Premultiplied,
						ret.address_of()
						)
					);
					//DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_ALPHA_MODE_PREMULTIPLIED, ret.address_of()));
				return ret;
				}()} {}
			};
		struct surface_interop : details::com_ptr<ABI::Windows::UI::Composition::ICompositionDrawingSurfaceInterop>
			{
			using com_ptr::com_ptr;
			surface_interop(const composition::surface& composition_surface) : com_ptr{[&composition_surface] {return composition_surface.as<self_t>(); }()} {}

			d2d::device_context begin_draw()
				{
				details::com_ptr<ID2D1DeviceContext> ret;

				POINT offset; // ?
				operator->()->BeginDraw(nullptr, __uuidof(ID2D1DeviceContext), ret.thing_for_uuidof_functions(), &offset);
				return {ret.as<d2d::device_context>()};
				}
			};*/
		}

	}