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
#include "hwnd_wrapper.h"
#include "details/style.h"

namespace utils::win32::window
	{
	struct initializer;

	using procedure_t = std::function<std::optional<LRESULT>(UINT msg, WPARAM wparam, LPARAM lparam)>;
	using procedures_container_t  = utils::containers::object_pool<procedure_t>;
	using procedure_handle_raw    = procedures_container_t::handle_raw;
	using procedure_handle_unique = procedures_container_t::handle_unique;
	using procedure_handle_shared = procedures_container_t::handle_shared;

	class base : public hwnd_wrapper, utils::oop::non_copyable, utils::oop::non_movable
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

			inline base(const create_info& create_info) : hwnd_wrapper{create_window(create_info)}
				{
				set_window_ptr();
				}
			inline virtual ~base() noexcept { if (get_handle()) { ::DestroyWindow(get_handle()); } }

		private:
			inline static constexpr wchar_t class_name[27]{L"CPP_Utilities Window Class"};

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

#pragma region procedure
		public:
			procedures_container_t procedures;

		private:
			void set_window_ptr() { SetWindowLongPtr(get_handle(), GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this)); }
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
					case WM_NCDESTROY: hwnd_wrapper::close(); return 0;
					}

				return DefWindowProc(get_handle(), msg, wparam, lparam);
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

	class module : utils::oop::non_copyable, utils::oop::non_movable
		{
		public:
			const base& get_base() const noexcept { return *base_ptr; }
			      base& get_base()       noexcept { return *base_ptr; }

		protected:
			module(window::base& base, procedure_t procedure) :
				base_ptr{&base},
				procedure_handle{base.procedures.make_unique(procedure)}
				{}

		private:
			const utils::observer_ptr<utils::win32::window::base> base_ptr;
			procedure_handle_unique procedure_handle;
		};

	}