#include "error_to_exception.h"

#include "windows.h"

namespace utils::MS
	{
	std::system_error last_error(const std::string& message)
		{
		return std::system_error{std::error_code(::GetLastError(), std::system_category()), message + '\n'};
		}
	}