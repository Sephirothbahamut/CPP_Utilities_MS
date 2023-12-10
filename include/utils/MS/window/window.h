#pragma once

#include <vector>
#include <memory>

#include <utils/memory.h>
#include <utils/math/vec2.h>
#include <utils/math/rect.h>
#include <utils/oop/disable_move_copy.h>
#include <utils/containers/object_pool.h>

#include "../base_types.h"
#include "hwnd_wrapper.h"


namespace utils::MS::window
	{
	class base;
	class procedure_result
		{
		friend class base;
		public:
			/// <summary> Prevents procedure from being called for modules after this one. The global procedure will return result. </summary>
			static procedure_result stop(LRESULT result) noexcept;
			/// <summary> If no other procedure after this one processed the message, the global procedure will return result. </summary>
			static procedure_result next(LRESULT result) noexcept;
			/// <summary> If no other procedure after this one processed the message, it will be passed to the default window procedure. </summary>
			static procedure_result next(              ) noexcept;

		private:
			procedure_result(bool halt, LRESULT result);
			procedure_result(bool halt                );
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

			const base& get_base() const noexcept;
			      base& get_base()       noexcept;

		protected:
			module(window::base& base);

			virtual procedure_result procedure(UINT msg, WPARAM wparam, LPARAM lparam);

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

	class base : public hwnd_wrapper
		{
		friend struct initializer;

		using modules_container_t = std::vector<std::unique_ptr<module>>;

		public:
#pragma region creation and destruction
			struct create_info
				{
				std::wstring title{L"Untitled Window"};
				DWORD style_ex{0};
				DWORD style   {window_styles::ws_overlappedwindow};
				std::optional<utils::math::vec2i> position{std::nullopt};
				std::optional<utils::math::vec2u> size    {std::nullopt};
				utils::observer_ptr<base> parent_window   {nullptr};
				};

			base(const create_info& create_info);

			template <concepts::module_create_info ...Args>
			base(create_info base_create_info, Args&&... other_create_infos) : base{adjust_create_info(base_create_info, std::forward<Args>(other_create_infos)...)}
				{
				(this->emplace_module<typename Args::module_type>(other_create_infos), ...);
				}

			base           (base&& move) noexcept;
			base& operator=(base&& move) noexcept;

			~base() noexcept;

		private:
			template <concepts::module_create_info ...Args>
			static create_info adjust_create_info(create_info base_create_info, Args&&... other_create_infos)
				{
				([&]
					{
					if constexpr (concepts::create_info_affecting_base<Args>)
						{
						other_create_infos.adjust_base_create_info(base_create_info);
						}
					}(), ...);
				return base_create_info;
				}

			void destroy_window_if_exists() noexcept;

			inline static constexpr wchar_t class_name[27]{L"CPP_Utilities Window Class"};

			HWND create_window(const create_info& create_info);
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
			void set_window_ptr();
			static utils::observer_ptr<base> get_window_ptr(HWND handle);

			static LRESULT __stdcall window_procedure(HWND handle, UINT msg, WPARAM wparam, LPARAM lparam) noexcept;

			LRESULT procedure(UINT msg, WPARAM wparam, LPARAM lparam) noexcept;
#pragma endregion procedure
#pragma region events
		public:
			bool poll_event() const;
			void wait_event() const;
#pragma endregion events
		};
		
	struct initializer : utils::oop::non_copyable, utils::oop::non_movable
		{
		 initializer();
		~initializer();
		};

	}

#ifdef utils_implementation
#include "window.cpp"
#endif