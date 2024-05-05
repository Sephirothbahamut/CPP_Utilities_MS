#pragma once
#include <string>
#include <system_error>

namespace utils::MS
	{
	std::system_error last_error(const std::string& message);
	}

#ifdef utils_implementation
#include "error_to_exception.cpp"
#endif