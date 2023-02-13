#include <iostream>
#include <chrono>

#include <utils/enum.h>

#include <utils/MS/window/window.h>
#include <utils/MS/window/style.h>
#include <utils/MS/window/regions.h>
#include <utils/MS/graphics/d2d.h>
#include <utils/MS/graphics/d2d/window.h>

#include "examples.h"

static void body()
	{
	using namespace utils::output;

	utils::MS::graphics::co::initializer co_initializer;
	utils::MS::graphics::d2d::factory d2d_factory;

	utils::MS::graphics::d3d::device d3d_device;
	utils::MS::graphics::dxgi::device dxgi_device{d3d_device};
	utils::MS::graphics::d2d::device d2d_device{d2d_factory, dxgi_device};

	utils::MS::window::initializer window_initializer;

	utils::MS::window::base window
		{
		utils::MS::window::base::create_info{.position{{1024, 768}}, .size{{256u, 128u}}},
		close_module::create_info{},
		utils::MS::window::style::create_info
			{
			.transparency{utils::MS::window::style::transparency_t::composition_attribute},
			.borders{utils::MS::window::style::value_t::disable}
			},
		utils::MS::window::resizable_edge::create_info{},
		//utils::MS::graphics::d2d::window::render_target::create_info         //transparency, cannot guarantee same device for multiple windows
		//utils::MS::graphics::d2d::window::swap_chain::create_info            //guarantees same device for multiple windows, no transparency
		utils::MS::graphics::d2d::window::composition_swap_chain::create_info //should do both but I don't trust I've done it correctly at all lol
			{
			//.d2d_factory{d2d_factory}, //decomment for render_target, since it doesn't take a device
			.d2d_device{d2d_device}, //decomment for the other 2
			.draw_callback
				{
				[](const utils::MS::window::base& window, const utils::MS::graphics::d2d::device_context& context)
					{
					context->SetTransform(D2D1::IdentityMatrix());
					context->Clear(D2D1_COLOR_F{0.f, 0.f, 0.f, 0.f});
					
					auto client_rect{window.client_rect};
					utils::math::vec2f half_size{static_cast<float>(client_rect.width()) / 2.f, static_cast<float>(client_rect.height()) / 2.f};
					
					utils::math::rect<float> top_left    {0, 0, half_size.x, half_size.y};
					utils::math::rect<float> top_right   {top_left }; top_right   .x() += half_size.x;
					utils::math::rect<float> bottom_left {top_left }; bottom_left .y() += half_size.y;
					utils::math::rect<float> bottom_right{top_right}; bottom_right.y() += half_size.y;
					
					utils::MS::graphics::details::com_ptr<ID2D1SolidColorBrush> brush_r;
					context->CreateSolidColorBrush(D2D1_COLOR_F{.r{1.f}, .g{0.f}, .b{0.f}, .a{.5f}}, brush_r.address_of());
					utils::MS::graphics::details::com_ptr<ID2D1SolidColorBrush> brush_g;
					context->CreateSolidColorBrush(D2D1_COLOR_F{.r{0.f}, .g{1.f}, .b{0.f}, .a{.5f}}, brush_g.address_of());
					utils::MS::graphics::details::com_ptr<ID2D1SolidColorBrush> brush_b;
					context->CreateSolidColorBrush(D2D1_COLOR_F{.r{0.f}, .g{0.f}, .b{1.f}, .a{.5f}}, brush_b.address_of());
					utils::MS::graphics::details::com_ptr<ID2D1SolidColorBrush> brush_y;
					context->CreateSolidColorBrush(D2D1_COLOR_F{.r{1.f}, .g{1.f}, .b{0.f}, .a{.5f}}, brush_y.address_of());
					
					context->FillRectangle(D2D1_RECT_F{.left{top_left    .ll}, .top{top_left    .up}, .right{top_left    .rr}, .bottom{top_left    .dw} }, brush_r.get());
					context->FillRectangle(D2D1_RECT_F{.left{top_right   .ll}, .top{top_right   .up}, .right{top_right   .rr}, .bottom{top_right   .dw} }, brush_g.get());
					//context->FillRectangle(D2D1_RECT_F{.left{bottom_left .ll}, .top{bottom_left .up}, .right{bottom_left .rr}, .bottom{bottom_left .dw} }, brush_b.get());
					context->FillRectangle(D2D1_RECT_F{.left{bottom_right.ll}, .top{bottom_right.up}, .right{bottom_right.rr}, .bottom{bottom_right.dw} }, brush_y.get());
					}
				}
			}
		};
		
	window.show();
	while (window.is_open())
		{
		window.wait_event();
		}
	}

void example::drawing_system()
	{
	try { body(); }
	catch (const std::system_error & e) { std::cout << e.what() << std::endl; }
	catch (const std::runtime_error& e) { std::cout << e.what() << std::endl; }
	}