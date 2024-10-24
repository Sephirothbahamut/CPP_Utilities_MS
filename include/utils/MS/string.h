#pragma once

#include <string>

namespace utils::MS::string
	{
	std::string  wide_to_utf8(const std::wstring& wstr) noexcept;
	std::wstring utf8_to_wide(const std::string & str ) noexcept;
	}

#ifdef utils_implementation
#include "string.cpp"
#endif