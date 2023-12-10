#pragma once
#include "window.h"

#include "../windows.h"
#include "../error_to_exception.h"

namespace utils::MS::window
	{
	procedure_result procedure_result::stop(LRESULT result) noexcept { return {true , result}; }
	procedure_result procedure_result::next(LRESULT result) noexcept { return {false, result}; }
	procedure_result procedure_result::next(              ) noexcept { return {false        }; }

	procedure_result::procedure_result(bool halt, LRESULT result) : halt{halt}, result{result      } {}
	procedure_result::procedure_result(bool halt                ) : halt{halt}, result{std::nullopt} {}

	const base& module::get_base() const noexcept { return *base_ptr; }
	      base& module::get_base()       noexcept { return *base_ptr; }

	module::module(window::base& base) : base_ptr{&base} {}

	procedure_result module::procedure(UINT msg, WPARAM wparam, LPARAM lparam) { return procedure_result::next(); }


	base::base(const create_info& create_info) : hwnd_wrapper{create_window(create_info)}
				{
				set_window_ptr();
				}


	base::base(base&& move) noexcept { operator=(std::move(move)); }
	base& base::operator=(base&& move) noexcept
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

	base::~base() noexcept { destroy_window_if_exists(); }

	void base::destroy_window_if_exists() noexcept
		{
		if (get_handle()) { ::DestroyWindow(get_handle()); }
		}

	inline static constexpr wchar_t class_name[27]{L"CPP_Utilities Window Class"};

	HWND base::create_window(const base::create_info& create_info)
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

	void base::set_window_ptr() { SetWindowLongPtr(get_handle(), GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this)); }
	utils::observer_ptr<base> base::get_window_ptr(HWND handle)
		{
		return reinterpret_cast<utils::observer_ptr<base>>(GetWindowLongPtr(handle, GWLP_USERDATA));
		}

	LRESULT __stdcall base::window_procedure(HWND handle, UINT msg, WPARAM wparam, LPARAM lparam) noexcept
		{
		auto window_ptr{get_window_ptr(handle)};
		if (window_ptr) { return window_ptr->procedure(msg, wparam, lparam); }
		else { return ::DefWindowProc(handle, msg, wparam, lparam); }
		}

	LRESULT base::procedure(UINT msg, WPARAM wparam, LPARAM lparam) noexcept
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
	
	bool base::poll_event() const
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

	void base::wait_event() const
		{
		MSG msg;
		bool ret{static_cast<bool>(GetMessage(&msg, get_handle(), 0, 0))}; //true if WM_QUIT, false otherwise
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
		}

	initializer::initializer()
		{
		auto hBrush{CreateSolidBrush(RGB(0, 0, 0))};
		WNDCLASSEXW wcx
			{
			.cbSize        {sizeof(wcx)                     },
			.style         {CS_HREDRAW | CS_VREDRAW         },
			.lpfnWndProc   {base::window_procedure          },
			.hInstance     {nullptr                         },
			.hCursor       {::LoadCursor(nullptr, IDC_ARROW)},
			.hbrBackground {hBrush                          },
			.lpszClassName {base::class_name},
			};
		RegisterClassExW(&wcx);
		}
	initializer::~initializer() { UnregisterClassW(base::class_name, nullptr); }
	}