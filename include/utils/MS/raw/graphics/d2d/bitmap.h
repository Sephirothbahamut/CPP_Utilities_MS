#pragma once

#include "common.h"

#include "context.h"
#include "../dxgi/swap_chain.h"

#include <utils/matrix.h>
#include <utils/math/vec.h>
#include <utils/graphics/colour.h>

namespace utils::MS::raw::graphics::d2d
	{
	struct bitmap : com_wrapper<ID2D1Bitmap1>
		{
		struct create_info
			{
			DXGI_FORMAT         dxgi_format       {DXGI_FORMAT_B8G8R8A8_UNORM   };
			D2D1_ALPHA_MODE     alpha_mode        {D2D1_ALPHA_MODE_PREMULTIPLIED};
			utils::math::vec2f  dpi               {1.f, 1.f};
			D2D1_BITMAP_OPTIONS options           {D2D1_BITMAP_OPTIONS_TARGET   };
			};

		bitmap() noexcept;
		bitmap(const d2d::context& d2d_context, const create_info& create_info, const utils::math::vec2s& size);
		bitmap(const d2d::context& d2d_context, const create_info& create_info);
		bitmap(const d2d::context& d2d_context, utils::math::vec2s size);
		bitmap(const d2d::context& d2d_context);

		bitmap(const d2d::context& d2d_context, const create_info& create_info, const dxgi::swap_chain& dxgi_swapchain);

		//bitmap(const d2d::device_context& d2d_context, const wic::bitmap& wic_bitmap) : com_ptr{create_from_wic_bitmap  (context, wic_bitmap)} {}

		utils::matrix<utils::graphics::colour::rgba_f> to_cpu_matrix(d2d::context& d2d_context) const;

		//inline static pointer create_from_wic_bitmap(const d2d::device_context& d2d_context, const wic::bitmap& wic_bitmap)
		//	{
		//	pointer ret{nullptr};
		//	details::throw_if_failed(d2d_context->CreateBitmapFromWicBitmap(wic_bitmap.get(), &ret));
		//	return ret;
		//	}
		};
	}

#ifdef utils_implementation
#include "bitmap.cpp"
#endif