#include "string.h"

#include "raw/windows.h"

namespace utils::MS::string
	{
	std::string wide_to_utf8(const std::wstring& wstr) noexcept
		{
		int count = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.length()), NULL, 0, NULL, NULL);
		std::string str(count, 0);
		WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], count, NULL, NULL);
		return str;
		}

	std::wstring utf8_to_wide(const std::string& str) noexcept
		{
		int count = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.length()), NULL, 0);
		std::wstring wstr(count, 0);
		MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.length()), &wstr[0], count);
		return wstr;
		}
	}