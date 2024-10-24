#pragma once

#include "common.h"

#include <utils/matrix.h>
#include <utils/math/vec.h>
#include <utils/graphics/colour.h>

namespace utils::MS::raw::graphics::d2d::bitmap
	{
	winrt::com_ptr<ID2D1Bitmap1> create()
		{
		}

	struct bitmap : ms_wrapper<ID2D1Bitmap1>
		{
		struct create_info
			{
			utils::math::vec2s resolution;
			DXGI_FORMAT dxgi_format;
			D2D1_ALPHA_MODE alpha_mode;
			utils::math::vec2f dpi{1.f, 1.f};
			D2D1_BITMAP_OPTIONS options;
			};

		using ms_wrapper<ID2D1Bitmap1>::ms_wrapper;
		bitmap(ID2D1DeviceContext* d2d_context, const create_info& create_info);

		utils::matrix<utils::graphics::colour::rgba_f> to_cpu_matrix(ID2D1DeviceContext* d2d_context);
		};
	}

#ifdef utils_implementation
#include "bitmap.cpp"
#endif