#pragma once

#include <string>
#include <iomanip>
#include <sstream>
#include <stdexcept>

#include <utils/oop/disable_move_copy.h>

#include "windows.h"
#include "../common.h"

#include <winrt/base.h>

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

	template <typename ms_type>
	struct ms_wrapper : utils::oop::non_copyable
		{
		//Disambiguate between ms_wrapper(ms_type* ms_ptr) and potential children constructors that take a pointer to something else.
		ms_wrapper(nullptr_t) : ptr{nullptr} {};
		ms_wrapper(ms_type* ms_ptr) : ptr{ms_ptr} {};
		ms_wrapper(ms_wrapper&& move) noexcept : ptr{move.ptr} { move.ptr = nullptr; }
		ms_wrapper& operator=(ms_wrapper&& move) noexcept
			{
			safe_release(ptr);
			ptr = move.ptr;
			move.ptr = nullptr;
			return *this;
			}

		~ms_wrapper() 
			{
			safe_release(ptr); 
			}

		ms_type* ptr{nullptr};
		ms_type* operator->() noexcept { return ptr; }
		operator ms_type*() noexcept { return ptr; }
		operator bool() const noexcept { return ptr; }
		};
	}