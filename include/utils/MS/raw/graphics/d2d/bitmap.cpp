#include "bitmap.h"

namespace utils::MS::raw::graphics::d2d::bitmap
	{
	com_ptr create(winrt::com_ptr<ID2D1DeviceContext> d2d_context, const create_info& create_info)
		{
		com_ptr ret;
		winrt::check_hresult(d2d_context->CreateBitmap(D2D1_SIZE_U{static_cast<uint32_t>(create_info.resolution.x()), static_cast<uint32_t>(create_info.resolution.y())},
			nullptr,
			static_cast<uint32_t>(create_info.resolution.x()) * 4,
			D2D1_BITMAP_PROPERTIES1
			{//DXGI_FORMAT_B8G8R8A8_UNORM
			.pixelFormat{.format{create_info.dxgi_format}, .alphaMode{create_info.alpha_mode}},
			.dpiX{create_info.dpi.x()},
			.dpiY{create_info.dpi.y()},
			.bitmapOptions{create_info.options}
			},
			ret.put()));
		return ret;
		}

	utils::matrix<utils::graphics::colour::rgba_f> bitmap::to_cpu_matrix(com_ptr bitmap, winrt::com_ptr<ID2D1DeviceContext> d2d_context)
		{
		if (bitmap->GetPixelFormat().format != DXGI_FORMAT_R32G32B32A32_FLOAT)
			{
			throw std::logic_error{"Operation only supported for DXGI_FORMAT_R32G32B32A32_FLOAT bitmaps"};
			}

		const auto pixel_size{bitmap->GetPixelSize()};
		utils::math::vec2s resolution{static_cast<size_t>(pixel_size.width ), static_cast<size_t>(pixel_size.height)};

		auto cpu_bitmap{create(d2d_context, bitmap::create_info
			{
			.resolution{resolution},
			.dxgi_format{DXGI_FORMAT_R32G32B32A32_FLOAT},//DXGI_FORMAT_B8G8R8A8_UNORM
			.alpha_mode {D2D1_ALPHA_MODE_PREMULTIPLIED},
			.options    {D2D1_BITMAP_OPTIONS_CPU_READ | D2D1_BITMAP_OPTIONS_CANNOT_DRAW}
			})};


		D2D1_POINT_2U copy_dest_point{0, 0};
		D2D1_RECT_U copy_src_rect{.left{0}, .top{0}, .right{static_cast<uint32_t>(resolution.x())}, .bottom{static_cast<uint32_t>(resolution.y())}};
		cpu_bitmap->CopyFromBitmap(&copy_dest_point, bitmap.get(), &copy_src_rect);

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
	}