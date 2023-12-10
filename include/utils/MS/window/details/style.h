#pragma once

#include <utils/math/rect.h>
#include "../../base_types.h"

namespace utils::MS::window::details::style
	{
	enum class Style : DWORD
		{
		windowed         = window_styles::ws_overlappedwindow | window_styles::ws_thickframe | window_styles::ws_caption | window_styles::ws_sysmenu | window_styles::ws_minimizebox | window_styles::ws_maximizebox,
		aero_borderless  = window_styles::ws_popup            | window_styles::ws_thickframe | window_styles::ws_caption | window_styles::ws_sysmenu | window_styles::ws_minimizebox | window_styles::ws_maximizebox,
		basic_borderless = window_styles::ws_popup            | window_styles::ws_thickframe | window_styles::ws_sysmenu                             | window_styles::ws_minimizebox | window_styles::ws_maximizebox
		};

	bool composition_enabled() noexcept;

	Style select_borderless_style() noexcept;

	void set_shadow(HWND handle, bool enabled);

	bool maximized(HWND hwnd);

	std::optional<utils::math::rect<long>> maximized_client_rect(HWND window);
	}

#ifdef utils_implementation
#include "style.cpp"
#endif