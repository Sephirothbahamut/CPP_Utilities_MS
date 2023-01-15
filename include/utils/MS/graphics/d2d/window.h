#pragma once
#include "../d2d.h"

#include "../../window/window.h"

namespace utils::MS::graphics::d2d::window
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
				using module_type = render_target;
				const d2d::factory& d2d_factory;
				std::function<on_draw_signature> on_render;
				
				inline void adjust_base_create_info(utils::MS::window::base::create_info& base_create_info) const noexcept
					{
					base_create_info.style_ex |= WS_EX_NOREDIRECTIONBITMAP;
					}
				};
	
			render_target(utils::MS::window::base& base, create_info create_info) :
				module{base},
				on_render{create_info.on_render},
				d2d_hwnd_rt{create_info.d2d_factory, get_base().get_handle()},
				d2d_device_context{d2d_hwnd_rt}
				{
				}
	
			std::function<on_draw_signature> on_render;
	
			void present() noexcept
				{
				}
	
		private:
			d2d::hwnd_render_target d2d_hwnd_rt;
			d2d::device_context d2d_device_context;
	
			virtual utils::MS::window::procedure_result procedure(UINT msg, WPARAM wparam, LPARAM lparam) override
				{
				switch (msg)
					{
					case WM_SIZE:
						on_resize({LOWORD(lparam), HIWORD(lparam)});
						return utils::MS::window::procedure_result::next(0);
	
					case WM_DISPLAYCHANGE:
						//InvalidateRect(get_handle(), NULL, FALSE);
						break;
					
					case WM_ERASEBKGND: return utils::MS::window::procedure_result::stop(1);

					case WM_PAINT:
						if (on_render)
							{
							on_render(get_base(), d2d_device_context);
							ValidateRect(get_base().get_handle(), NULL);
							return utils::MS::window::procedure_result::next(0);
							}
						break;
	
					}
	
				return utils::MS::window::procedure_result::next();
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
				using module_type = swap_chain;

				const d2d ::device& d2d_device;
				std ::function<on_draw_signature> on_render;

				inline void adjust_base_create_info(utils::MS::window::base::create_info& base_create_info) const noexcept
					{
					base_create_info.style_ex |= WS_EX_NOREDIRECTIONBITMAP;
					}
				};

			swap_chain(utils::MS::window::base& base, create_info create_info) :
				module{base},
				on_render{create_info.on_render},
				d2d_device_context{create_info.d2d_device},
				dxgi_swapchain{create_info.d2d_device.get_dxgi_device(), get_base().get_handle()},
				d2d_bitmap_target{d2d_device_context, dxgi_swapchain}
				{
				d2d_device_context->SetTarget(d2d_bitmap_target.get());
				}

			std::function<on_draw_signature> on_render;

		private:
			d2d::device_context d2d_device_context;
			dxgi::swap_chain dxgi_swapchain;
			d2d::bitmap d2d_bitmap_target;


			virtual utils::MS::window::procedure_result procedure(UINT msg, WPARAM wparam, LPARAM lparam) override
				{
				switch (msg)
					{
					case WM_SIZE:
						on_resize({LOWORD(lparam), HIWORD(lparam)});
						return utils::MS::window::procedure_result::next(0);

					case WM_DISPLAYCHANGE:
						//InvalidateRect(get_handle(), NULL, FALSE);
						break;

					case WM_ERASEBKGND: return utils::MS::window::procedure_result::stop(1);

					case WM_PAINT:
						if (on_render)
							{
							on_render(get_base(), d2d_device_context);
							dxgi_swapchain.present();

							ValidateRect(get_base().get_handle(), NULL);
							return utils::MS::window::procedure_result::next(0);
							}
						break;

					}

				return utils::MS::window::procedure_result::next();
				}

			void on_resize(utils::math::vec2u size)
				{
				//Release things that reference the swapchain before resizing
				d2d_device_context->SetTarget(nullptr);
				d2d_bitmap_target.reset();

				dxgi_swapchain.resize(size);

				//re-get back buffer
				d2d_bitmap_target = d2d::bitmap{d2d_device_context, dxgi_swapchain};
				d2d_device_context->SetTarget(d2d_bitmap_target.get());
				}
		};
	}