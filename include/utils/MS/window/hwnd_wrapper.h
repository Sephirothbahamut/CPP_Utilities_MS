#pragma once

#include <utils/math/vec2.h>
#include <utils/math/vec2.h>
#include <utils/math/rect.h>
#include <utils/oop/disable_move_copy.h>

#include "../base_types.h"

namespace utils::MS::window
	{
	using rect_t = utils::math::rect<long>;

	/// <summary>
	/// Wraps existing HWND windows. Used to get properties of the windows with my own interface, wrapping raw Win32 API calls and types.
	/// </summary>
	class hwnd_wrapper : utils::oop::non_copyable
		{
		public:
			hwnd_wrapper(HWND handle = nullptr);
			hwnd_wrapper           (hwnd_wrapper&& move) noexcept;
			hwnd_wrapper& operator=(hwnd_wrapper&& move) noexcept;

			void show();

			bool is_open     () const noexcept;
			void close       ()       noexcept;
			void minimize    ()       noexcept;
			void maximize    ()       noexcept;
			void restore     ()       noexcept;

			bool is_visible  () const noexcept;
			bool is_minimized() const noexcept;
			bool is_maximized() const noexcept;

			hwnd_wrapper get_parent() const noexcept;

			const HWND get_handle() const noexcept;

#pragma region rects
			rect_t get_window_rect(           ) const noexcept;
			void   set_window_rect(rect_t rect)       noexcept;
			rect_t get_client_rect(           ) const noexcept;
			void   set_client_rect(rect_t rect)       noexcept;

			__declspec(property(get = get_window_rect, put = set_window_rect)) rect_t window_rect;
			__declspec(property(get = get_client_rect, put = set_client_rect)) rect_t client_rect;

			rect_t get_shadow_thickness() const noexcept;

			/// <summary> 
			/// Takes the rect representing window size (with borders and title bar if present), and adds the shadow (if present) thickness.
			/// Windows APIs that deal with window rects (set/get position/side) include the shadows.
			/// Users tendentially don't give a fluff about the shadows.
			/// </summary>
			rect_t add_shadow_size_to(rect_t rect) const noexcept;

			/// <summary> 
			/// Takes the Win32 rect representing window size + shadow (with borders and title bar if present) and removes the shadow.
			/// Windows APIs that deal with window rects (set/get position/side) include the shadows.
			/// Users tendentially don't give a fluff about the shadows.
			/// </summary>
			rect_t remove_shadow_size_from(rect_t rect) const noexcept;

#pragma endregion

		private:
			HWND handle;
		};

	}

#ifdef utils_implementation
#include "hwnd_wrapper.cpp"
#endif