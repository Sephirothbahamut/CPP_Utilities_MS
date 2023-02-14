#pragma once
#include "../d2d.h"

#include <Windows.Graphics.DirectX.Direct3D11.interop.h>
#include <Windows.ui.composition.interop.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Core.h>

#include "../../window/window.h"

namespace utils::MS::graphics::d2d::window
	{
	using draw_callback_signature = void(const utils::MS::window::base&, const d2d::device_context&);

	namespace details
		{
		template <typename DERIVED_T>
		class base : public utils::MS::window::module
			{
			private:
				using derived_t = DERIVED_T;
				constexpr const derived_t& derived() const noexcept { return static_cast<const derived_t&>(*this); }
				constexpr       derived_t& derived()       noexcept { return static_cast<      derived_t&>(*this); }

			public:
				base(utils::MS::window::base& window_base, std::function<draw_callback_signature> draw_callback) :
					module{window_base},
					draw_callback{draw_callback}
					{}

				std::function<draw_callback_signature> draw_callback;

				bool draw(std::function<draw_callback_signature> draw_callback) const noexcept {};
				bool draw() const noexcept { return derived().draw(draw_callback); }

			protected:
				void on_resize(utils::math::vec2u size) noexcept {}

				virtual utils::MS::window::procedure_result procedure(UINT msg, WPARAM wparam, LPARAM lparam) override
					{
					switch (msg)
						{
						case WM_SIZE:
							derived().on_resize({LOWORD(lparam), HIWORD(lparam)});
							return utils::MS::window::procedure_result::next(0);
	
						case WM_DISPLAYCHANGE:
							//InvalidateRect(get_handle(), NULL, FALSE);
							break;
					
						case WM_ERASEBKGND: return utils::MS::window::procedure_result::stop(1);

						case WM_PAINT:
							if (draw()) 
								{
								ValidateRect(get_base().get_handle(), nullptr);
								return utils::MS::window::procedure_result::next(0); 
								}
							break;
						}
	
					return utils::MS::window::procedure_result::next();
					}
			};
		}


	/// <summary>
	/// Simplest version that works with window transparency. 
	/// You have no control over devices usage so you cannot ensure that multiple windows share the same device. 
	/// This means you cannot use the same resources across multiple windows created this way.
	/// </summary>
	class render_target : public details::base<render_target>
		{
		// Getting a "modern" context from an "outdated" render_target is undocumented, but according to Paint.NET's creator it works
		// https://stackoverflow.com/questions/32883780/how-to-render-child-window-with-direct2d-in-native-desktop-windows-application/32977109#32977109
		// and indeed it's working way more easily and without headaches.
		friend class details::base<render_target>;
		public:
	
			struct create_info
				{
				using module_type = render_target;
				const d2d::factory& d2d_factory;
				std::function<draw_callback_signature> draw_callback;
				
				inline void adjust_base_create_info(utils::MS::window::base::create_info& base_create_info) const noexcept
					{
					base_create_info.style_ex |= WS_EX_NOREDIRECTIONBITMAP;
					base_create_info.style    |= CS_HREDRAW | CS_VREDRAW;
					}
				};
	
			render_target(utils::MS::window::base& base, create_info create_info) :
				details::base<render_target>{base, create_info.draw_callback},
				d2d_hwnd_rt{create_info.d2d_factory, get_base().get_handle()},
				d2d_device_context{d2d_hwnd_rt}
				{
				}

			bool draw(std::function<draw_callback_signature> draw_callback) const noexcept
				{
				if (!draw_callback) { return false; }
				d2d_device_context->BeginDraw();
				draw_callback(get_base(), d2d_device_context);
				d2d_device_context->EndDraw();
				return true;
				}

		private:
			d2d::hwnd_render_target d2d_hwnd_rt;
			d2d::device_context d2d_device_context;

			void on_resize(utils::math::vec2u size) noexcept 
				{
				d2d_hwnd_rt->Resize({size.x, size.y});
				}
		};

	/// <summary>
	/// Proper modern way to create a swapchain for directx drawing.
	/// *HOWEVER* window transparency is not supported at all.
	/// If you don't need transparency this is the cleanest solution.
	/// </summary>
	class swap_chain : public details::base<swap_chain>
		{
		// This is the proper way, it doesn't work with transparentcy in the swapchain
		// seemingly only works with DXGI_ALPHA_MODE_IGNORE, in particular doesn't work with DXGI_ALPHA_MODE_PREMULTIPLIED
		// no idea how to get it to work
		// but since the rendertarget way works with already half the lines of code, screw dxgi swapchain I guess
		friend class details::base<swap_chain>;
		public:
			struct create_info
				{
				using module_type = swap_chain;

				const d2d::device& d2d_device;
				std::function<draw_callback_signature> draw_callback;

				inline void adjust_base_create_info(utils::MS::window::base::create_info& base_create_info) const noexcept
					{
					base_create_info.style_ex |= WS_EX_NOREDIRECTIONBITMAP;
					base_create_info.style    |= CS_HREDRAW | CS_VREDRAW;
					}
				};

			swap_chain(utils::MS::window::base& base, create_info create_info) :
				details::base<swap_chain>{base, create_info.draw_callback},
				d2d_device_context{create_info.d2d_device},
				dxgi_swapchain{create_info.d2d_device.get_dxgi_device(), get_base().get_handle()},
				d2d_bitmap_target{d2d_device_context, dxgi_swapchain}
				{
				d2d_device_context->SetTarget(d2d_bitmap_target.get());
				}

			bool draw() const noexcept { details::base<swap_chain>::draw(); }
			bool draw(std::function<draw_callback_signature> draw_callback) const noexcept
				{
				if (!draw_callback) { return false; }
				d2d_device_context->BeginDraw();
				draw_callback(get_base(), d2d_device_context);
				d2d_device_context->EndDraw();
				dxgi_swapchain.present();
				return true;
				}

		private:
			d2d::device_context d2d_device_context;
			dxgi::swap_chain dxgi_swapchain;
			d2d::bitmap d2d_bitmap_target;

			void on_resize(utils::math::vec2u size) noexcept
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

	/// <summary>
	/// Modern (I think) correct way to create a directx-drawable window that supports transparency through composition APIs.
	/// Resizing with this transparency feels less "snappy" than the render_target alternative, 
	/// but you have control over which devices are used which means you can share resources across multiple windows.
	/// </summary>
	class composition_swap_chain : public details::base<composition_swap_chain>
		{
		friend class details::base<composition_swap_chain>;
		public:
			struct create_info
				{
				using module_type = composition_swap_chain;

				const d2d::device& d2d_device;
				std::function<draw_callback_signature> draw_callback;

				inline void adjust_base_create_info(utils::MS::window::base::create_info& base_create_info) const noexcept
					{
					base_create_info.style_ex |= WS_EX_NOREDIRECTIONBITMAP;
					base_create_info.style    |= CS_HREDRAW | CS_VREDRAW;
					}
				};

			composition_swap_chain(utils::MS::window::base& base, create_info create_info) :
				details::base<composition_swap_chain>{base, create_info.draw_callback},
				d2d_device_context{create_info.d2d_device},
				composition_device{create_info.d2d_device.get_dxgi_device()},
				composition_visual{composition_device},
				composition_target{composition_device, get_base().get_handle()},
				dxgi_swapchain    {create_info.d2d_device.get_dxgi_device(), get_base().get_handle(), nullptr}
				{}

			bool draw(std::function<draw_callback_signature> draw_callback) const noexcept
				{
				if (!draw_callback) { return false; }

				d2d::bitmap bitmap{d2d_device_context, dxgi_swapchain};
				d2d_device_context->SetTarget(bitmap.get());

				d2d_device_context->BeginDraw();

				draw_callback(get_base(), d2d_device_context);

				d2d_device_context->EndDraw();
				dxgi_swapchain    ->Present(1, 0);
				composition_visual->SetContent(dxgi_swapchain.get());
				composition_target->SetRoot(composition_visual.get());
				composition_device->Commit();

				return true;
				}

		private:
			d2d::device_context d2d_device_context;
			composition::device composition_device;
			composition::visual composition_visual;
			composition::target composition_target;
			dxgi::swap_chain dxgi_swapchain;

			void on_resize(utils::math::vec2u size) noexcept
				{
				d2d_device_context->SetTarget(nullptr);
				dxgi_swapchain.resize(size);
				}
		};
	}