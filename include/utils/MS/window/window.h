#pragma once
// IMPORTANT: If you include this header, remember to define WIN32_LEAN_AND_MEAN and NOMINMAX

#include <vector>
#include <string>
#include <optional>
#include <concepts>
#include <functional>

#include <utils/memory.h>
#include <utils/math/vec2.h>
#include <utils/math/rect.h>
#include <utils/oop/disable_move_copy.h>
#include <utils/containers/object_pool.h>

#include "../windows.h"

#include "../error_to_exception.h"
#include "hwnd_wrapper.h"

namespace utils::MS::window
	{
	class base;
	class procedure_result
		{
		friend class base;
		public:
			/// <summary> Prevents procedure from being called for modules after this one. The global procedure will return result. </summary>
			inline static procedure_result stop(LRESULT result) noexcept { return {true , result}; }
			/// <summary> If no other procedure after this one processed the message, the global procedure will return result. </summary>
			inline static procedure_result next(LRESULT result) noexcept { return {false, result}; }
			/// <summary> If no other procedure after this one processed the message, it will be passed to the default window procedure. </summary>
			inline static procedure_result next(              ) noexcept { return {false        }; }

		private:
			procedure_result(bool halt, LRESULT result) : halt{halt}, result{result      } {}
			procedure_result(bool halt                ) : halt{halt}, result{std::nullopt} {}
			bool halt;
			std::optional<LRESULT> result;
		};


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
			virtual ~module() {};

			const base& get_base() const noexcept { return *base_ptr; }
			      base& get_base()       noexcept { return *base_ptr; }

		protected:
			module(window::base& base) : base_ptr{&base} {}

			virtual procedure_result procedure(UINT msg, WPARAM wparam, LPARAM lparam) { return procedure_result::next(); }

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
					{t.adjust_base_create_info(base_create_info)}-> std::same_as<void>;
			};
		};

	class base : public hwnd_wrapper, utils::oop::non_copyable
		{
		friend struct initializer;

		using modules_container_t = std::vector<std::unique_ptr<module>>;

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
						other_create_infos.adjust_base_create_info(base_create_info);
						}
					}(), ...);

				(this->emplace_module<typename Args::module_type>(other_create_infos), ...);
				}

			base(base&& move) noexcept { operator=(std::move(move)); }
			base& operator=(base&& move) noexcept
				{
				destroy_window_if_exists();
				hwnd_wrapper::operator=(std::move(move));

				set_window_ptr();

				modules = std::move(move.modules);

				for (auto& module : modules)
					{
					module->base_ptr = this;
					}
				return *this;
				}

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
			T& emplace_module(Args&&... args)
				{
				auto unique_ptr{std::make_unique<T>(*this, std::forward<Args>(args)...)};
				utils::observer_ptr<T> ret_ptr{unique_ptr.get()};
				modules.emplace_back(std::move(unique_ptr));
				return *ret_ptr;
				}
			template <concepts::module_create_info T>
			T::module_type& emplace_module_from_create_info(const T& create_info)
				{
				return emplace_module<typename T::module_type>(create_info);
				}

			template <concepts::module T>
			utils::observer_ptr<T> get_module_ptr() noexcept
				{
				for (auto& module_ptr : modules)
					{
					if (auto cast_ptr{dynamic_cast<utils::observer_ptr<T>>(module_ptr.get())})
						{
						return cast_ptr;
						}
					}
				return nullptr;
				}
			template <concepts::module T>
			utils::observer_ptr<const T> get_module_ptr() const noexcept
				{
				for (auto& module_ptr : modules)
					{
					if (auto cast_ptr{dynamic_cast<utils::observer_ptr<const T>>(module_ptr.get())})
						{
						return cast_ptr;
						}
					}
				return nullptr;
				}

			template <concepts::module T>
			void remove_module() noexcept
				{
				std::erase_if(modules, [](const std::unique_ptr<module>& module_ptr) { return dynamic_cast<utils::observer_ptr<const T>>(module_ptr.get()); });
				}

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
				std::optional<LRESULT> last_result{std::nullopt};
				
				for (auto& module : modules)
					{
					auto ret{module->procedure(msg, wparam, lparam)};
					if (ret.result) { last_result = ret.result; }
					if (ret.halt  ) { break; }
					}
				
				switch (msg)
					{
					case WM_NCDESTROY: hwnd_wrapper::close(); return 0;
					}

				if (last_result) { return last_result.value(); }
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