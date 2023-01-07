#pragma once
// IMPORTANT: If you include this header, remember to define WIN32_LEAN_AND_MEAN and NOMINMAX

#include <vector>
#include <string>
#include <optional>
#include <functional>

#include <utils/math/vec2.h>
#include <utils/memory.h>
#include <utils/math/vec2.h>
#include <utils/math/geometry/aabb.h>
#include <utils/oop/disable_move_copy.h>
#include <utils/containers/object_pool.h>

#include "../windows.h"

#include "../error_to_exception.h"
#include "details/style.h"

namespace utils::win32::window
	{
	using rect_t = utils::math::rect<long>;

	namespace details
		{
		rect_t win32_RECT_to_rect_t(const RECT& rect)
			{
			return {.ll{rect.left}, .up{rect.top}, .rr{rect.right}, .dw{rect.bottom}};
			}
		RECT rect_t_to_win32_RECT(const rect_t& rect)
			{
			return {.left{rect.ll}, .top{rect.up}, .right{rect.rr}, .bottom{rect.dw}};
			}
		}

	/// <summary>
	/// Wraps existing HWND windows. Used to get properties of the windows with my own interface, wrapping raw Win32 API calls and types.
	/// </summary>
	class hwnd_wrapper
		{
		public:
			hwnd_wrapper(HWND handle = nullptr) : handle{handle} {}

			inline void show()
				{
				::SetWindowPos(handle, nullptr, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
				::ShowWindow(handle, SW_SHOW);
				}

			inline bool is_open () const noexcept { return handle; }
			inline void close   () noexcept { ::DestroyWindow(get_handle()); handle = nullptr;}
			inline void minimize() noexcept { ::ShowWindow(get_handle(), SW_MINIMIZE); }
			inline void maximize() noexcept { ::ShowWindow(get_handle(), SW_MAXIMIZE); }

			inline bool is_visible() const noexcept
				{
				bool is_cloaked{false};
				DwmGetWindowAttribute(get_handle(), DWMWA_CLOAKED, &is_cloaked, sizeof(is_cloaked));
				return IsWindowVisible(get_handle()) && !is_cloaked && !is_minimized();
				}

			inline bool is_minimized() const noexcept { return IsIconic(get_handle()); }
			inline bool is_maximized() const noexcept { return IsZoomed(get_handle()); }

			inline hwnd_wrapper get_parent() const noexcept
				{
				return {GetParent(get_handle())};
				}

			inline const HWND get_handle() const noexcept { return handle; }

#pragma region rects
			rect_t get_window_rect() const noexcept
				{
				RECT rect_win32;
				if (DwmGetWindowAttribute(get_handle(), DWMWA_EXTENDED_FRAME_BOUNDS, &rect_win32, sizeof(RECT)) == S_OK) 
					{
					//Equivalent to using unpack_window_size on GetWindowRect
					return details::win32_RECT_to_rect_t(rect_win32);
					}
				else if (GetWindowRect(get_handle(), &rect_win32))
					{
					return details::win32_RECT_to_rect_t(rect_win32);
					}
				else
					{
					//TODO error case
					return {};
					}
				}
			void set_window_rect(rect_t rect) noexcept
				{
				RECT rectangle{pack_window_size(rect)};

				long width {rectangle.right  - rectangle.left};
				long height{rectangle.bottom - rectangle.top };

				SetWindowPos(handle, NULL, rectangle.left, rectangle.top, width, height, SWP_NOZORDER);
				}
			rect_t get_client_rect() const noexcept
				{
				RECT rect;
				GetClientRect(handle, &rect);//TODO error case
				return details::win32_RECT_to_rect_t(rect);
				}
			void set_client_rect(rect_t rect) noexcept
				{
				RECT rectangle{details::rect_t_to_win32_RECT(rect)};
				
				// Updates the rect to take into account border/title bar if present
				AdjustWindowRectEx(&rectangle, static_cast<DWORD>(GetWindowLongPtr(handle, GWL_STYLE)), false, static_cast<DWORD>(GetWindowLongPtr(handle, GWL_EXSTYLE)));

				long width {rectangle.right  - rectangle.left};
				long height{rectangle.bottom - rectangle.top };

				SetWindowPos(handle, NULL, rectangle.left, rectangle.top, width, height, SWP_NOZORDER);
				}

			__declspec(property(get = get_window_rect, put = set_window_rect)) rect_t window_rect;
			__declspec(property(get = get_client_rect, put = set_client_rect)) rect_t client_rect;

			rect_t get_shadow_thickness() const noexcept
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

			/// <summary> 
			/// Takes the rect representing window size (with borders and title bar if present), and adds the shadow (if present) thickness.
			/// Windows APIs that deal with window rects (set/get position/side) include the shadows.
			/// Users tendentially don't give a fluff about the shadows.
			/// </summary>
			RECT pack_window_size(rect_t rect) const noexcept
				{
				const auto shadow_thickness{get_shadow_thickness()};
				return
					{
					.left   = rect.ll + shadow_thickness.ll,
					.top    = rect.up + shadow_thickness.up,
					.right  = rect.rr + shadow_thickness.rr,
					.bottom = rect.dw + shadow_thickness.dw
					};
				}

			/// <summary> 
			/// Takes the Win32 rect representing window size + shadow (with borders and title bar if present) and removes the shadow.
			/// Windows APIs that deal with window rects (set/get position/side) include the shadows.
			/// Users tendentially don't give a fluff about the shadows.
			/// </summary>
			rect_t unpack_window_size(RECT rect) const noexcept
				{
				const auto shadow_thickness{get_shadow_thickness()};
				return
					{
					.ll = rect.left   - shadow_thickness.ll,
					.up = rect.top    - shadow_thickness.up,
					.rr = rect.right  - shadow_thickness.rr,
					.dw = rect.bottom - shadow_thickness.dw
					};
				}
#pragma endregion

		private:
			HWND handle;
		};

	}