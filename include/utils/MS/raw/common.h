#pragma once

#include <string>
#include <system_error>

#include "windows.h"

namespace utils::MS::raw
	{
	bool succeeded(HRESULT result) noexcept;
	bool failed   (HRESULT result) noexcept;

	//std::string hr_to_string(HRESULT hr) noexcept;

	//void throw_if_failed(HRESULT hr);	//use winrt::check_hresult instead

	std::system_error last_error(const std::string& message);
	}

#ifdef utils_implementation
#include "common.cpp"
#endif