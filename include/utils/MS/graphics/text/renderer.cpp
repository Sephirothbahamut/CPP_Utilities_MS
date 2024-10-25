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

//#include <dcomp.h>
//#include <d2d1_3.h>
//#include <d3d11_3.h>
//#include <dwrite_3.h>
//#include <wincodec.h>
//#include <wrl/client.h>
//#include <windows.ui.composition.interop.h>

#include <utils/memory.h>

//#pragma comment(lib, "d2d1")
//#pragma comment(lib, "dxgi")
//#pragma comment(lib, "dcomp")
//#pragma comment(lib, "d3d11")
//#pragma comment(lib, "dwrite")
//#pragma comment(lib, "windowscodecs")

#include "../../string.h"
#include "../../raw/graphics/dxgi.h"
#include "../../raw/graphics/d2d.h"
#include "../../raw/graphics/d3d.h"
#include "../../raw/graphics/dw.h"

namespace umrg = utils::MS::raw::graphics;

namespace utils::MS::graphics::text
	{
	struct renderer::implementation : utils::oop::non_copyable, utils::oop::non_movable
		{
		utils::math::vec2s  resolution{size_t{0}, size_t{0}};
		umrg::d2d ::factory::com_ptr d2d_factory{nullptr};
		umrg::dw  ::factory::com_ptr dw_factory {nullptr};
		umrg::d3d ::device ::com_ptr d3d_device {nullptr};
		umrg::dxgi::device ::com_ptr dxgi_device{nullptr};
		umrg::d2d ::device ::com_ptr d2d_device {nullptr};
		umrg::d2d ::context::com_ptr d2d_context{nullptr};
		umrg::d2d ::bitmap ::com_ptr gpu_bitmap {nullptr};

		implementation() :
			d2d_factory{umrg::d2d ::factory::create()},
			dw_factory {umrg::dw  ::factory::create()},
			d3d_device {umrg::d3d ::device ::create()},
			dxgi_device{umrg::dxgi::device ::create(d3d_device)},
			d2d_device {umrg::d2d ::device ::create(d2d_factory, dxgi_device)},
			d2d_context{umrg::d2d ::context::create(d2d_device)},
			gpu_bitmap {nullptr}
			{}
		implementation(const utils::math::vec2s& resolution, const utils::graphics::colour::rgba_f& clear_colour = utils::graphics::colour::rgba_f{0.f}) :
			implementation{}
			{
			reset(resolution, clear_colour);
			}
		
		void clear(const utils::graphics::colour::rgba_f& colour = utils::graphics::colour::rgba_f{0.f})
			{
			if (!gpu_bitmap) { return; }

			d2d_context->BeginDraw();
			d2d_context->Clear(D2D1_COLOR_F{.r{colour.r()}, .g{colour.g()}, .b{colour.b()}, .a{colour.a()}});
			winrt::check_hresult(d2d_context->EndDraw());
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

			gpu_bitmap = umrg::d2d::bitmap::create(d2d_context, umrg::d2d::bitmap::create_info
				{
				.resolution{resolution},
				.dxgi_format{DXGI_FORMAT_R32G32B32A32_FLOAT},//DXGI_FORMAT_B8G8R8A8_UNORM
				.alpha_mode {D2D1_ALPHA_MODE_PREMULTIPLIED},
				.options    {D2D1_BITMAP_OPTIONS_TARGET}
				});
			d2d_context->SetTarget(gpu_bitmap.get());
			}

		void draw_text(const format& format, const std::string& string, const utils::math::rect<float> region)
			{
			if (!gpu_bitmap) { return; }

			const std::wstring wstring{utils::MS::string::utf8_to_wide(string)};

			D2D1_RECT_F layoutRect = D2D1_RECT_F{.left{region.ll()}, .top{region.up()}, .right{region.rr()}, .bottom{region.dw()}};

			auto brush{umrg::d2d::brush::create(d2d_context, format.colour)};

			auto dw_format{umrg::dw::text_format::create(dw_factory, format)};
			auto dw_layout{umrg::dw::text_layout::create(dw_factory, dw_format, string, region.size())};
			
			if (format.shrink_to_fit)
				{
				float current_size{dw_layout->GetFontSize()};
				DWRITE_TEXT_METRICS metrics;
				while (true)
					{
					dw_layout->GetMetrics(&metrics);
					if (current_size > 1.f && metrics.height > metrics.layoutHeight)
						{
						current_size -= 1.f;
						dw_layout->SetFontSize(current_size, DWRITE_TEXT_RANGE{.startPosition{0}, .length{static_cast<unsigned int>(wstring.size())}});
						}
					else { break; }
					}

				//Test
				//utils::MS::graphics::text::format shrinked_format{format};
				//shrinked_format.size = current_size;
				//auto shrinked_dw_format{umrg::dw::text_format::create(dw_factory, format)};
				//auto shrinked_dw_layout{umrg::dw::text_layout::create(dw_factory, shrinked_dw_format, string, region.size())};
				//
				//d2d_context->BeginDraw();
				//d2d_context->DrawTextLayout(umrg::d2d::cast(region.ul()), shrinked_dw_layout.get(), brush.get(), D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT | D2D1_DRAW_TEXT_OPTIONS_CLIP);
				////d2d_context->DrawTextW(wstring.c_str(), static_cast<UINT32>(wstring.size()), dw_format, layoutRect, brush);
				//winrt::check_hresult(d2d_context->EndDraw());
				//return;
				}


			d2d_context->BeginDraw();
			d2d_context->DrawTextLayout(umrg::d2d::cast(region.ul()), dw_layout.get(), brush.get(), D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT | D2D1_DRAW_TEXT_OPTIONS_CLIP);
			//d2d_context->DrawTextW(wstring.c_str(), static_cast<UINT32>(wstring.size()), dw_format, layoutRect, brush);
			winrt::check_hresult(d2d_context->EndDraw());
			}

		utils::matrix<utils::graphics::colour::rgba_f> get_image()
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
			cpu_bitmap->CopyFromBitmap(&copy_dest_point, gpu_bitmap.get(), &copy_src_rect);

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
		};

	renderer::renderer() : implementation_ptr{std::make_unique<renderer::implementation>()} {}
	renderer::renderer(const utils::math::vec2s& resolution, const utils::graphics::colour::rgba_f& clear_colour) : 
		implementation_ptr{std::make_unique<renderer::implementation>(resolution, clear_colour)} {}

	renderer::~renderer() = default;

	void renderer::clear(const utils::graphics::colour::rgba_f& colour) { implementation_ptr->clear(colour); }
	void renderer::reset(const utils::math::vec2s& resolution, const utils::graphics::colour::rgba_f& clear_colour) { implementation_ptr->reset(resolution, clear_colour); }
	void renderer::draw_text(const format& format, const std::string& string, const utils::math::rect<float>& region) { implementation_ptr->draw_text(format, string, region); }

	utils::matrix<utils::graphics::colour::rgba_f> renderer::get_image() const { return implementation_ptr->get_image(); }
	}