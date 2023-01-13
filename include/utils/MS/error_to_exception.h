#pragma once
#include <string>
#include <system_error>

#include "windows.h"

namespace utils::MS
	{
	inline std::system_error last_error(const std::string& message)
		{
		return std::system_error{std::error_code(::GetLastError(), std::system_category()), message + '\n'};
		}
	}