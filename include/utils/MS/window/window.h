#pragma once
// IMPORTANT: If you include this header, remember to define WIN32_LEAN_AND_MEAN and NOMINMAX

#include <vector>
#include <string>
#include <optional>
#include <concepts>
#include <functional>

#include <utils/memory.h>
#include <utils/math/vec2.h>
#include <utils/math/geometry/aabb.h>
#include <utils/oop/disable_move_copy.h>
#include <utils/containers/object_pool.h>

#include "../windows.h"

#include "../error_to_exception.h"
#include "hwnd_wrapper.h"

namespace utils::MS::window
	{
	/// <summary>
	/// Window modules should inherit from module, and implement the procedure if necessary.
	/// Window modules constructors should take a reference to MS::window::base as first parameters.
	/// With all the previous criteria, your module can be emplaced into the window.
	/// 
	/// If your module needs to change your window's style or ex_style on creation, please implement the 2 methods you can see in the create_info structure of this library's style module.
	/// 
	/// Optionally, your module may have a constructor that takes a reference to MS::window::base and a custom struct with all the necessary parameters.
	/// That struct should have a type declaration as follows: "using module_type = [your module class name]".
	/// If you do this, the module can be automatically emplaced on window construction by passing the custom struct of your module as additional parameter on window construction.
	/// </summary>
	class module
		{
		friend class base;
		public:
			const base& get_base() const noexcept { return *base_ptr; }
			      base& get_base()       noexcept { return *base_ptr; }

		protected:
			module(window::base& base) : base_ptr{&base} {}

			virtual std::optional<LRESULT> procedure(UINT msg, WPARAM wparam, LPARAM lparam) { return std::nullopt; }

		private:
			utils::observer_ptr<utils::MS::window::base> base_ptr;
		};

	struct initializer;

	using procedure_signature = std::optional<LRESULT>(UINT msg, WPARAM wparam, LPARAM lparam);
	namespace concepts
		{
		template <typename T>
		concept module = std::derived_from<T, utils::MS::window::module>;
		template <typename T>
		concept module_create_info = std::same_as<T, typename T::module_type::create_info> && concepts::module<typename T::module_type>;
		template <typename T>
		concept create_info_affecting_base = module_create_info<T> && requires(T t, base::create_info& base_create_info)
			{
					{t.adjust_base_create_info(base_create_info)}-> std::same_as<base::create_info>;
			};
		};

	class base : public hwnd_wrapper, utils::oop::non_copyable, utils::oop::non_movable
		{
		friend class initializer;

		using modules_container_t = utils::containers::object_pool<std::unique_ptr<module>, 8Ui64, true, false, false>;
		template <typename T>
		class module_handle : public modules_container_t::handle_raw
			{
			public:
				using value_type        = T;
				using reference         = value_type& ;
				using const_reference   = const reference;
				using pointer           = value_type* ;
				using const_pointer     = const pointer;
				using iterator_category = std::random_access_iterator_tag;
				using difference_type   = ptrdiff_t ;

				//note: these handles are created by methods which either instantiate T or made a dynamic_cast check to T.

					  reference operator* ()       noexcept { return reinterpret_cast<reference      >(*get_module_ptr()); }
				const_reference operator* () const noexcept { return reinterpret_cast<const_reference>(*get_module_ptr()); }
					  pointer   operator->()       noexcept { return reinterpret_cast<      pointer  >( get_module_ptr()); }
				const_pointer   operator->() const noexcept { return reinterpret_cast<const_pointer  >( get_module_ptr()); }
					  reference value     ()                { return operator* (); }
				const_reference value     () const          { return operator* (); }
					  pointer   get       ()       noexcept { return operator->(); }
				const_pointer   get       () const noexcept { return operator->(); }

			private:
				      utils::observer_ptr<module> get_module_ptr()       noexcept { return modules_container_t::handle_raw::operator*().get(); }
				const utils::observer_ptr<module> get_module_ptr() const noexcept { return modules_container_t::handle_raw::operator*().get(); }
			};

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
			template <concepts::module_create_info ...Args>
			inline base(create_info base_create_info, Args&&... other_create_infos) : base{base_create_info}
				{
				([&]
					{
					if constexpr (concepts::create_info_affecting_base<Args>)
						{
						base_create_info = other_create_infos.adjust_base_create_info(base_create_info);
						}
					}, ...);

				(this->emplace_module<typename Args::module_type>(other_create_infos), ...);
				}

			//TODO object pool's move operations
			// remove this class's inheritance from non_movable
			// add move operator=
			//base(base&& move) noexcept
			//	{
			//	destroy_window_if_exists();
			//	hwnd_wrapper::operator=(std::move(move));
			//
			//	set_window_ptr();
			//
			//	modules = std::move(move.modules);
			//
			//	for (auto& module: modules)
			//		{
			//		module->base_ptr = this;
			//		}
			//	}

			inline ~base() noexcept { destroy_window_if_exists(); }

		private:
			void destroy_window_if_exists() noexcept
				{
				if (get_handle()) { ::DestroyWindow(get_handle()); }
				}

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

				if (!ret) { throw last_error("Failed to create window. Did you forget to create an initializer instance for your window class? (utils::MS::window::initializer)"); }

				return ret;
				}
#pragma endregion creation and destruction

#pragma region modules
		public:
			template <concepts::module T, typename ...Args>
			module_handle<T> emplace_module(Args&&... args)
				{
				return {modules.emplace(std::make_unique<T>(*this, std::forward<Args>(args)...))};
				}
			template <concepts::module_create_info T>
			module_handle<typename T::module_type> emplace_module_from_create_info(const T& create_info)
				{
				return emplace_module<typename T::module_type>(create_info);
				}

			template <concepts::module T>
			module_handle<T> get_module_handle() noexcept
				{
				for (auto it{modules.begin()}; it != modules.end(); it++)
					{
					if (dynamic_cast<utils::observer_ptr<T>>((*it).operator->()))
						{
						return {modules.handle_to_iterator(it)};
						}
					}
				return {};
				}
		private:
			modules_container_t modules;
#pragma endregion modules

#pragma region procedure
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
				for (auto& module : modules)
					{
					if (auto ret{module->procedure(msg, wparam, lparam)})
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
					bool ret{static_cast<bool>(PeekMessage(&msg, get_handle(), 0, 0, PM_REMOVE))};
					if (ret)
						{
						::TranslateMessage(&msg);
						::DispatchMessage(&msg);
						return true;
						}
					else { return false; }
					}
				inline void wait_event() const
					{
					MSG msg;
					bool ret{static_cast<bool>(GetMessage(&msg, get_handle(), 0, 0))}; //true if WM_QUIT, false otherwise
					::TranslateMessage(&msg);
					::DispatchMessage(&msg);
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