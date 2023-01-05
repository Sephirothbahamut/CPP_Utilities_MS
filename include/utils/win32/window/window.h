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
	struct initializer;

	using rect_t = utils::math::rect<long>;

	class base : utils::oop::non_copyable, utils::oop::non_movable
		{
		friend class initializer;
		public:
#pragma region creation and destruction
			struct create_info
				{
				std::wstring title{L"Untitled Window"};
				DWORD style_ex{0};
				DWORD style   {WS_OVERLAPPEDWINDOW};
				std::optional<utils::math::vec2i> position{std::nullopt};
				std::optional<utils::math::vec2u> size    {std::nullopt};
				utils::observer_ptr<base> parent_window   {nullptr};
				};

			inline base(const create_info& create_info) : handle{create_window(create_info)}
				{
				set_window_ptr();
				}
			inline virtual ~base() noexcept { if (handle) { ::DestroyWindow(handle); } }

			inline void show()
				{
				::SetWindowPos(handle, nullptr, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
				::ShowWindow(handle, SW_SHOW);
				}

		private:
			inline static constexpr wchar_t class_name[27]{L"CPP_Utilities Window Class"};
			HWND handle{nullptr};

			HWND create_window(const create_info& create_info)
				{
				HWND ret = ::CreateWindowExW
				(
					create_info.style_ex, 
					class_name, 
					create_info.title.c_str(),
					create_info.style, 
					create_info.position.has_value() ? create_info.position.value().x : CW_USEDEFAULT,
					create_info.position.has_value() ? create_info.position.value().y : CW_USEDEFAULT,
					create_info.size    .has_value() ? create_info.size    .value().x : CW_USEDEFAULT,
					create_info.size    .has_value() ? create_info.size    .value().y : CW_USEDEFAULT,
					create_info.parent_window ? create_info.parent_window->get_handle() : nullptr,
					nullptr, nullptr, this
				);

				if (!ret) { throw last_error("Failed to create window. Did you forget to create an initializer instance for your window class? (utils::win32::window::initializer)"); }

				return ret;
				}
#pragma endregion creation and destruction
#pragma region properties
		public:
			inline bool is_open () const noexcept { return handle; }
			inline void close   () noexcept { ::DestroyWindow(get_handle()); }
			inline void minimize() noexcept { ::ShowWindow(get_handle(), SW_MINIMIZE); }
			inline void maximize() noexcept { ::ShowWindow(get_handle(), SW_MAXIMIZE); }

			inline const HWND get_handle() const noexcept { return handle; }

			rect_t get_window_rect() const noexcept
				{
				RECT rect;
				GetWindowRect(handle, &rect);
				return {.ll{rect.left}, .up{rect.top}, .rr{rect.right}, .dw{rect.bottom}};
				}
			void set_window_rect(rect_t rect) noexcept
				{
				// SetWindowPos wants the total size of the window (including title bar and borders),
				// so we have to compute it
				RECT rectangle = {.left{rect.ll}, .top{rect.up}, .right{rect.rr}, .bottom{rect.down}};
				AdjustWindowRect(&rectangle, static_cast<DWORD>(GetWindowLongPtr(handle, GWL_STYLE)), false);

				long width  = rectangle.right  - rectangle.left;
				long height = rectangle.bottom - rectangle.top;

				SetWindowPos(handle, NULL, rectangle.left, rectangle.top, width, height, SWP_NOZORDER);
				}
			rect_t get_client_rect() const noexcept
				{
				RECT rect;
				GetClientRect(handle, &rect);
				return {.ll{rect.left}, .up{rect.top}, .rr{rect.right}, .dw{rect.bottom}};
				}

			__declspec(property(get = get_window_rect, put = set_window_rect)) rect_t window_rect;
			__declspec(property(get = get_client_rect                       )) rect_t client_rect;
#pragma endregion properties

#pragma region procedure
		public:
			using procedures_container_t = utils::containers::object_pool<std::function<std::optional<LRESULT>(UINT msg, WPARAM wparam, LPARAM lparam)>>;
			procedures_container_t procedures;
			using procedure_handle_raw    = procedures_container_t::handle_raw;
			using procedure_handle_unique = procedures_container_t::handle_unique;
			using procedure_handle_shared = procedures_container_t::handle_shared;

		private:
			void set_window_ptr() { SetWindowLongPtr(handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this)); }
			inline static utils::observer_ptr<base> get_window_ptr(HWND handle)
				{
				return reinterpret_cast<utils::observer_ptr<base>>(GetWindowLongPtr(handle, GWLP_USERDATA));
				}

			inline static LRESULT __stdcall window_procedure(HWND handle, UINT msg, WPARAM wparam, LPARAM lparam) noexcept
				{
				auto window_ptr{get_window_ptr(handle)};
				if (window_ptr) { return window_ptr->procedure(msg, wparam, lparam); }
				else { return ::DefWindowProc(handle, msg, wparam, lparam); }
				}

			inline LRESULT procedure(UINT msg, WPARAM wparam, LPARAM lparam) noexcept
				{
				for (auto& procedure : procedures)
					{
					if (auto ret{procedure(msg, wparam, lparam)})
						{
						return ret.value();
						}
					}

				switch (msg)
					{
					case WM_NCDESTROY: handle = nullptr; return 0;
					}

				return DefWindowProc(handle, msg, wparam, lparam);
				}
#pragma endregion procedure
#pragma region events
			public:
				inline bool poll_event() const
					{
					MSG msg;
					bool ret = PeekMessage(&msg, get_handle(), 0, 0, PM_REMOVE);
					if (ret)
						{
						::TranslateMessage(&msg);
						::DispatchMessage(&msg);
						return true;
						}
					else { return false; }
					}
				inline bool wait_event() const
					{
					MSG msg;
					bool ret = GetMessage(&msg, get_handle(), 0, 0);
					if (ret)
						{
						::TranslateMessage(&msg);
						::DispatchMessage(&msg);
						return true;
						}
					else { return false; }
					}
#pragma endregion events
		};
		
	struct initializer : utils::oop::non_copyable, utils::oop::non_movable
		{
		inline initializer()
			{
			auto hBrush{CreateSolidBrush(RGB(0, 0, 0))};
			WNDCLASSEXW wcx
				{
				.cbSize        {sizeof(wcx)                      },
				.style         {CS_HREDRAW | CS_VREDRAW          },
				.lpfnWndProc   {base::window_procedure           },
				.hInstance     {nullptr                          },
				.hCursor       {::LoadCursorW(nullptr, IDC_ARROW)},
				.hbrBackground {hBrush                           },
				.lpszClassName {base::class_name},
				};
			RegisterClassEx(&wcx);
			}
		inline ~initializer() { UnregisterClass(base::class_name, nullptr); }
		};

	}