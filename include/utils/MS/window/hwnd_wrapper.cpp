#pragma once
#include "hwnd_wrapper.h"

#include "../windows.h"
#include "../cast.h"

#include "../error_to_exception.h"
#include "details/style.h"

#include "details/DWM.h"


#include "../base_types.h"

static_assert(std::same_as<::HWND, utils::MS::HWND>);

namespace utils::MS::window
	{
	hwnd_wrapper::hwnd_wrapper(HWND handle) : handle{handle} {}
	hwnd_wrapper::hwnd_wrapper           (hwnd_wrapper&& move) noexcept : handle{handle} { move.handle = nullptr; }
	hwnd_wrapper& hwnd_wrapper::operator=(hwnd_wrapper&& move) noexcept { handle = move.handle; move.handle = nullptr; return *this; }

	void hwnd_wrapper::show()
		{
		::SetWindowPos(handle, nullptr, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
		::ShowWindow(handle, SW_SHOW);
		}

	bool hwnd_wrapper::is_open () const noexcept { return handle; }
	void hwnd_wrapper::close   () noexcept { ::DestroyWindow(get_handle()); handle = nullptr;}
	void hwnd_wrapper::minimize() noexcept { ::ShowWindow(get_handle(), SW_MINIMIZE); }
	void hwnd_wrapper::maximize() noexcept { ::ShowWindow(get_handle(), SW_MAXIMIZE); }
	void hwnd_wrapper::restore () noexcept { ::ShowWindow(get_handle(), SW_RESTORE ); }

	bool hwnd_wrapper::is_visible() const noexcept
		{
		bool is_cloaked{false};
		DwmGetWindowAttribute(get_handle(), DWMWA_CLOAKED, &is_cloaked, sizeof(is_cloaked));
		return IsWindowVisible(get_handle()) && !is_cloaked && !is_minimized();
		}

	bool hwnd_wrapper::is_minimized() const noexcept { return IsIconic(get_handle()); }
	bool hwnd_wrapper::is_maximized() const noexcept { return IsZoomed(get_handle()); }

	hwnd_wrapper hwnd_wrapper::get_parent() const noexcept
		{
		return {GetParent(get_handle())};
		}

	const HWND hwnd_wrapper::get_handle() const noexcept { return handle; }

#pragma region rects
	rect_t hwnd_wrapper::get_window_rect() const noexcept
		{
		RECT rect_win32;
		if (DwmGetWindowAttribute(get_handle(), DWMWA_EXTENDED_FRAME_BOUNDS, &rect_win32, sizeof(RECT)) == S_OK) 
			{
			//Equivalent to using unpack_window_size on GetWindowRect
			return cast(rect_win32);
			}
		else if (GetWindowRect(get_handle(), &rect_win32))
			{
			return cast(rect_win32);
			}
		else
			{
			//TODO error case
			return {};
			}
		}
	void hwnd_wrapper::set_window_rect(rect_t rect) noexcept
		{
		const RECT rectangle{cast(add_shadow_size_to(rect))};
		
		const long width {rectangle.right  - rectangle.left};
		const long height{rectangle.bottom - rectangle.top };

		SetWindowPos(handle, NULL, rectangle.left, rectangle.top, width, height, SWP_NOZORDER);
		}
	rect_t hwnd_wrapper::get_client_rect() const noexcept
		{
		RECT rect;
		GetClientRect(handle, &rect);//TODO error case
		return cast(rect);
		}
	void hwnd_wrapper::set_client_rect(rect_t rect) noexcept
		{
		RECT rectangle{cast(rect)};
				
		// Updates the rect to take into account border/title bar if present
		AdjustWindowRectEx(&rectangle, static_cast<DWORD>(GetWindowLongPtr(handle, GWL_STYLE)), false, static_cast<DWORD>(GetWindowLongPtr(handle, GWL_EXSTYLE)));

		long width {rectangle.right  - rectangle.left};
		long height{rectangle.bottom - rectangle.top };

		SetWindowPos(handle, NULL, rectangle.left, rectangle.top, width, height, SWP_NOZORDER);
		}

	rect_t hwnd_wrapper::get_shadow_thickness() const noexcept
		{
		RECT exclude_shadow;
		RECT include_shadow;

		DwmGetWindowAttribute(get_handle(), DWMWA_EXTENDED_FRAME_BOUNDS, &exclude_shadow, sizeof(RECT));
		GetWindowRect(get_handle(), &include_shadow);

		return
			{
			.ll = include_shadow.left   - exclude_shadow.left  , // As of Win10 value = -7 if shadows enabled for hwnd, 0 otherwise.
			.up = include_shadow.top    - exclude_shadow.top   , // As of Win10 value =  0 if shadows enabled for hwnd, 0 otherwise.
			.rr = include_shadow.right  - exclude_shadow.right , // As of Win10 value = +7 if shadows enabled for hwnd, 0 otherwise.
			.dw = include_shadow.bottom - exclude_shadow.bottom, // As of Win10 value = +7 if shadows enabled for hwnd, 0 otherwise.
			};
		}
	
	rect_t hwnd_wrapper::add_shadow_size_to(rect_t rect) const noexcept
		{
		const auto shadow_thickness{get_shadow_thickness()};
		return
			{
			.ll = rect.ll + shadow_thickness.ll,
			.up = rect.up + shadow_thickness.up,
			.rr = rect.rr + shadow_thickness.rr,
			.dw = rect.dw + shadow_thickness.dw
			};
		}

	rect_t hwnd_wrapper::remove_shadow_size_from(rect_t rect) const noexcept
		{
		const auto shadow_thickness{get_shadow_thickness()};
		return
			{
			.ll = rect.ll - shadow_thickness.ll,
			.up = rect.up - shadow_thickness.up,
			.rr = rect.rr - shadow_thickness.rr,
			.dw = rect.dw - shadow_thickness.dw
			};
		}

#pragma endregion
	}