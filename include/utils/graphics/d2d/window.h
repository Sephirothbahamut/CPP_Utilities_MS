#pragma once
#include "../d2d.h"

#include "../../MS/window/window.h"

namespace d2d::window
	{
	class render_target : public utils::MS::window::module
		{
		// Getting a "modern" context from an "outdated" render_target is undocumented, but according to Paint.NET's creator it works
		// https://stackoverflow.com/questions/32883780/how-to-render-child-window-with-direct2d-in-native-desktop-windows-application/32977109#32977109
		// and indeed it's working way more easily and without headaches.
		
		public:
			using on_draw_signature = void(utils::MS::window::base&, const d2d::device_context&);
	
			struct create_info
				{
				const d2d::factory& d2d_factory;
				std::function<on_draw_signature> on_render;
				};
	
			render_target(utils::MS::window::base& base, create_info create_info) :
				module{base},
				on_render{create_info.on_render},
				d2d_hwnd_rt{create_info.d2d_factory, get_base().get_handle()},
				d2d_device_context{d2d_hwnd_rt}
				{
				record_procedure([this](UINT msg, WPARAM wparam, LPARAM lparam) -> std::optional<LRESULT> { return procedure(msg, wparam, lparam); });
				}
	
			std::function<on_draw_signature> on_render;
	
			void present() noexcept
				{
				}
	
		private:
			d2d::hwnd_render_target d2d_hwnd_rt;
			d2d::device_context d2d_device_context;
	
			std::optional<LRESULT> procedure(UINT msg, WPARAM wparam, LPARAM lparam)
				{
				switch (msg)
					{
					case WM_SIZE:
						on_resize({LOWORD(lparam), HIWORD(lparam)});
						break;
	
					case WM_DISPLAYCHANGE:
						//InvalidateRect(get_handle(), NULL, FALSE);
						break;
	
					case WM_PAINT:
						if (on_render)
							{
							on_render(get_base(), d2d_device_context);
							ValidateRect(get_base().get_handle(), NULL);
							return 0;
							}
						break;
	
					}
	
				return std::nullopt;
				}
	
			void on_resize(utils::math::vec2u size)
				{
				d2d_hwnd_rt->Resize({size.x, size.y});
				}
		};

	class swap_chain : public utils::MS::window::module
		{
		// This is the proper way, it doesn't work with transparentcy in the swapchain
		// seemingly only works with DXGI_ALPHA_MODE_IGNORE, in particular doesn't work with DXGI_ALPHA_MODE_PREMULTIPLIED
		// no idea how to get it to work
		// but since the rendertarget way works with already half the lines of code, screw dxgi swapchain I guess
		
		public:
			using on_draw_signature = void(utils::MS::window::base&, const d2d::device_context&);

			struct create_info
				{
				const d3d ::device& d3d_device;
				const d2d ::device& d2d_device;
				const dxgi::device& dxgi_device;
				std ::function<on_draw_signature> on_render;
				};

			swap_chain(utils::MS::window::base& base, create_info create_info) :
				module{base},
				on_render{create_info.on_render},
				d2d_device_context{create_info.d2d_device},
				dxgi_swapchain{create_info.d3d_device, get_base().get_handle()},
				d2d_bitmap_target{d2d_device_context, dxgi_swapchain}
				{
				d2d_device_context->SetTarget(d2d_bitmap_target.get());
				record_procedure([this](UINT msg, WPARAM wparam, LPARAM lparam) -> std::optional<LRESULT> { return procedure(msg, wparam, lparam); });
				}

			std::function<on_draw_signature> on_render;

		private:
			d2d::device_context d2d_device_context;
			dxgi::swap_chain dxgi_swapchain;
			d2d::bitmap d2d_bitmap_target;


			std::optional<LRESULT> procedure(UINT msg, WPARAM wparam, LPARAM lparam)
				{
				switch (msg)
					{
					case WM_SIZE:
						on_resize({LOWORD(lparam), HIWORD(lparam)});
						break;

					case WM_DISPLAYCHANGE:
						//InvalidateRect(get_handle(), NULL, FALSE);
						break;

					case WM_PAINT:
						if (on_render)
							{
							on_render(get_base(), d2d_device_context);
							dxgi_swapchain.present();

							ValidateRect(get_base().get_handle(), NULL);
							return 0;
							}
						break;

					}

				return std::nullopt;
				}

			void on_resize(utils::math::vec2u size)
				{
				//Release things that reference the swapchain before resizing
				d2d_device_context->SetTarget(nullptr);
				d2d_bitmap_target.Reset();

				dxgi_swapchain.resize(size);

				//re-get back buffer
				d2d_bitmap_target = d2d::bitmap{d2d_device_context, dxgi_swapchain};
				d2d_device_context->SetTarget(d2d_bitmap_target.get());
				}
		};
	}