#pragma once

#include <string>

#include "windows.h"

namespace utils::MS::raw
	{
	bool succeeded(HRESULT result) noexcept;
	bool failed(HRESULT result) noexcept;

	//std::string hr_to_string(HRESULT hr) noexcept;

	//void throw_if_failed(HRESULT hr);
	//use winrt::check_hresult instead
	}

#ifdef utils_implementation
#include "common.cpp"
#endif