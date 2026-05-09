#pragma once

#include "common.h"
#include "factory.h"


namespace utils::MS::raw::graphics::d2d::hwnd_render_target
	{
	using com_ptr = winrt::com_ptr<ID2D1HwndRenderTarget>;

	struct create_info
		{
		HWND hwnd;
		D2D1_RENDER_TARGET_PROPERTIES properties
			{
			.type{D2D1_RENDER_TARGET_TYPE_DEFAULT},
			.pixelFormat
				{
				.format{DXGI_FORMAT_UNKNOWN},
				.alphaMode{D2D1_ALPHA_MODE_PREMULTIPLIED}
				}
			};
		};

	com_ptr create(d2d::factory::com_ptr& d2d_factory, const create_info& create_info);
	}

#ifdef utils_implementation
#include "render_target.cpp"
#endif