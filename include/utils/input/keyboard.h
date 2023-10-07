#pragma once

#include <unordered_map>
#include "common.h"

namespace utils::input
	{
	struct keyboard
		{
		std::unordered_map<size_t, input_1d<bool>>
		};
	}