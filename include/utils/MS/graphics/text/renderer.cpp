#include "renderer.h"

#include <memory>
#include <string>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <filesystem>

#include <utils/matrix.h>
#include <utils/math/rect.h>
#include <utils/math/rect.h>
#include <utils/graphics/colour.h>
#include <utils/compilation/debug.h>
#include <utils/oop/disable_move_copy.h>

#include "../dx.h"
#include "../dx/initializer/exposed_MS.h"

#include "../../string.h"
#include "../../raw/graphics/d2d.h"
#include "../../raw/graphics/text/custom_renderer/renderer.h"

#include "formatted_string_exposed_MS.h"


namespace umrg = utils::MS::raw::graphics;

namespace utils::MS::graphics::text
	{
	struct renderer::implementation
		{
		utils::math::vec2s resolution;
		umrg::dw::factory::com_ptr dw_factory;
		umrg::text::custom_renderer::renderer::com_ptr dw_renderer;
		umrg::d2d::context::com_ptr d2d_context;
		umrg::d2d::bitmap::com_ptr d2d_bitmap;
		umrg::text::custom_renderer::contexts contexts;

		implementation(dx::initializer& dx_initializer, const utils::math::vec2s& resolution, const utils::graphics::colour::rgba_f& clear_colour = utils::graphics::colour::rgba_f{0.f}) :
			dw_factory {dx_initializer.implementation_ptr->dw_factory},
			dw_renderer{umrg::text::custom_renderer::renderer::create(dx_initializer.implementation_ptr->d2d_factory)},
			d2d_context{umrg::d2d::context::create(dx_initializer.implementation_ptr->d2d_device)},
			contexts{.render_context{d2d_context}}
			{
			reset(resolution, clear_colour);
			}
		
		void clear(const utils::graphics::colour::rgba_f& colour = utils::graphics::colour::rgba_f{0.f})
			{
			if (!d2d_bitmap) { return; }

			d2d_context->BeginDraw();
			d2d_context->Clear(D2D1_COLOR_F{.r{colour.r()}, .g{colour.g()}, .b{colour.b()}, .a{colour.a()}});
			winrt::check_hresult(d2d_context->EndDraw());

			contexts.outlines      .clear();
			contexts.strikethroughs.clear();
			contexts.underlines    .clear();
			}

		void reset(const utils::math::vec2s& resolution, const utils::graphics::colour::rgba_f& clear_colour = utils::graphics::colour::rgba_f{0.f})
			{
			create_bitmap(resolution);
			clear(clear_colour);
			}

		void create_bitmap(const utils::math::vec2s& resolution)
			{
			this->resolution = resolution;
			d2d_context->SetTarget(nullptr);

			d2d_bitmap = umrg::d2d::bitmap::create(d2d_context, umrg::d2d::bitmap::create_info
				{
				.resolution{resolution},
				.dxgi_format{DXGI_FORMAT_R32G32B32A32_FLOAT},//DXGI_FORMAT_B8G8R8A8_UNORM
				.alpha_mode {D2D1_ALPHA_MODE_PREMULTIPLIED},
				.options    {D2D1_BITMAP_OPTIONS_TARGET}
				});
			d2d_context->SetTarget(d2d_bitmap.get());
			}

		void draw_text(const format& format, const std::string& string, const utils::math::rect<float> region)
			{
			if (!d2d_bitmap) { return; }

			const std::wstring wstring{utils::MS::string::utf8_to_wide(string)};

			D2D1_RECT_F layoutRect = D2D1_RECT_F{.left{region.ll()}, .top{region.up()}, .right{region.rr()}, .bottom{region.dw()}};

			auto brush{umrg::d2d::brush::create(d2d_context, utils::graphics::colour::rgba_f{0.f, 0.f, 0.f, 1.f})};

			auto dw_format{umrg::dw::text_format::create(dw_factory, format)};
			
			d2d_context->BeginDraw();
			d2d_context->DrawTextW(wstring.c_str(), static_cast<UINT32>(wstring.size()), dw_format.get(), layoutRect, brush.get());
			winrt::check_hresult(d2d_context->EndDraw());
			}

		void draw_text(const formatted_string& text, const utils::math::vec2f position)
			{
			const auto& dw_layout{text.implementation_ptr->dw_layout};
			d2d_context->BeginDraw();
			dw_layout->Draw(&contexts, dw_renderer.get(), position.x(), position.y());
			d2d_context->EndDraw();
			}


		utils::matrix<utils::graphics::colour::rgba_f> get_image() const
			{
			auto cpu_bitmap{umrg::d2d::bitmap::create(d2d_context, umrg::d2d::bitmap::create_info
				{
				.resolution{resolution},
				.dxgi_format{DXGI_FORMAT_R32G32B32A32_FLOAT},//DXGI_FORMAT_B8G8R8A8_UNORM
				.alpha_mode {D2D1_ALPHA_MODE_PREMULTIPLIED},
				.options    {D2D1_BITMAP_OPTIONS_CPU_READ | D2D1_BITMAP_OPTIONS_CANNOT_DRAW}
				})};

			
			D2D1_POINT_2U copy_dest_point{0, 0};
			D2D1_RECT_U copy_src_rect{.left{0}, .top{0}, .right{static_cast<uint32_t>(resolution.x())}, .bottom{static_cast<uint32_t>(resolution.y())}};
			cpu_bitmap->CopyFromBitmap(&copy_dest_point, d2d_bitmap.get(), &copy_src_rect);

			D2D1_MAPPED_RECT mapped_rect;
			winrt::check_hresult(cpu_bitmap->Map(D2D1_MAP_OPTIONS_READ, &mapped_rect));
			cpu_bitmap->Unmap();
	
			const size_t per_channel_bytes_count{sizeof(float)};
			const size_t per_pixel_bytes_count{per_channel_bytes_count * 4};
			const size_t per_row_bytes_count{per_pixel_bytes_count * resolution.x()};

			utils::matrix<utils::graphics::colour::rgba_f> ret{resolution};

			for (size_t y = 0; y < resolution.y(); y++)
				{
				const size_t row_begin_index{per_row_bytes_count * y};
				for (size_t x = 0; x < resolution.x(); x++)
					{
					const size_t current_pixel_begin_index{row_begin_index + (per_pixel_bytes_count * x)};

					const size_t r_index{current_pixel_begin_index};
					const size_t g_index{r_index + per_channel_bytes_count};
					const size_t b_index{g_index + per_channel_bytes_count};
					const size_t a_index{b_index + per_channel_bytes_count};

					const BYTE* r_byte_ptr{mapped_rect.bits + r_index};
					const BYTE* g_byte_ptr{mapped_rect.bits + g_index};
					const BYTE* b_byte_ptr{mapped_rect.bits + b_index};
					const BYTE* a_byte_ptr{mapped_rect.bits + a_index};

					const float* r_ptr{reinterpret_cast<const float*>(r_byte_ptr)};
					const float* g_ptr{reinterpret_cast<const float*>(g_byte_ptr)};
					const float* b_ptr{reinterpret_cast<const float*>(b_byte_ptr)};
					const float* a_ptr{reinterpret_cast<const float*>(a_byte_ptr)};

					const float r{*r_ptr};
					const float g{*g_ptr};
					const float b{*b_ptr};
					const float a{*a_ptr};

					auto& ret_pixel{ret[utils::math::vec2s{x, y}]};
					ret_pixel.r() = r;
					ret_pixel.g() = g;
					ret_pixel.b() = b;
					ret_pixel.a() = a;
					}
				}

			return ret;
			}
		
		output get_output() const
			{
			const output ret
				{
				.image         {get_image()            },
				.outlines      {contexts.outlines      },
				.strikethroughs{contexts.strikethroughs},
				.underlines    {contexts.underlines    },
				};
			return ret;
			}
		};

	renderer::renderer(dx::initializer& dx_initializer, const utils::math::vec2s& resolution, const utils::graphics::colour::rgba_f& clear_colour) :
		implementation_ptr{utils::make_polymorphic_value<renderer::implementation>(dx_initializer, resolution, clear_colour)} {}

	renderer::~renderer() = default;

	void renderer::clear(const utils::graphics::colour::rgba_f& colour) { implementation_ptr->clear(colour); }
	void renderer::reset(const utils::math::vec2s& resolution, const utils::graphics::colour::rgba_f& clear_colour) { implementation_ptr->reset(resolution, clear_colour); }
	void renderer::draw_text(const format& format, const std::string& string, const utils::math::rect<float>& region) { implementation_ptr->draw_text(format, string, region); }
	void renderer::draw_text(const formatted_string& text, const utils::math::vec2f position) { implementation_ptr->draw_text(text, position); }
	
	const region::rendering& renderer::get_default_rendering_properties() const noexcept { return implementation_ptr->dw_renderer->get_default_rendering_properties(); }
	      region::rendering& renderer::get_default_rendering_properties()       noexcept { return implementation_ptr->dw_renderer->get_default_rendering_properties(); }

	output renderer::get_output() const { return implementation_ptr->get_output(); }
	}