#include "common.h"

#include <string>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace utils::MS::raw
	{
	bool succeeded(HRESULT result) noexcept { return SUCCEEDED(result); }
	bool failed(HRESULT result) noexcept { return FAILED(result); }

	std::string hr_to_string(HRESULT hr) noexcept
		{
		std::stringstream ss;
		ss << std::hex << hr;
		return ss.str();
		}

	void throw_if_failed(HRESULT hr)
		{
		if (failed(hr))
			{
			const std::string error{hr_to_string(hr)};
			throw std::runtime_error{"Error code : " + error};
			}
		}
	}