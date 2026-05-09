#pragma once

#include <string>
#include <iomanip>
#include <sstream>
#include <stdexcept>

#include <utils/oop/disable_move_copy.h>

#include "windows.h"
#include "../common.h"

#include <winrt/base.h>
#pragma comment (lib, "runtimeobject.lib")

namespace utils::MS::raw::graphics
	{
	template <typename interface_type>
	void safe_release(interface_type*& instance)
		{
		if (instance != nullptr)
			{
			(instance)->Release();
			instance = nullptr;
			}
		}

	template <typename T, typename ...Args>
	winrt::com_ptr<T> create_com_ptr(Args&&... args)
		{
		// Not sure if this is the proper way to instantiate a COM interface
		// I thought i needed CoCreatesomething
		// But microsoft's own examples do it like this:
		// https://github.com/microsoft/Windows-classic-samples/blob/d338bb385b1ac47073e3540dbfa810f4dcb12ed8/Samples/Win7Samples/multimedia/DirectWrite/HelloWorld/CustomText.cpp#L379
		
		winrt::com_ptr<T> ret;

		T* raw_ptr{new (std::nothrow) T{std::forward<Args>(args)...}};
		
		*ret.put() = raw_ptr;

		return ret;
		}

	template <typename ms_type>
	struct com_wrapper
		{
		using com_wrapper_t = com_wrapper<ms_type>;
		using com_ptr_t = winrt::com_ptr<ms_type>;
		com_ptr_t com_ptr;

		auto operator->()       noexcept { return com_ptr.operator->(); }
		auto operator->() const noexcept { return com_ptr.operator->(); }
		auto get       ()       noexcept { return com_ptr.get       (); }
		auto get       () const noexcept { return com_ptr.get       (); }
		operator bool() const noexcept { return com_ptr.operator bool(); }
		};
	}