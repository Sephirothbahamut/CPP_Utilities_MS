#pragma once

#include <utils/polymorphic_value.h>

namespace utils::MS::graphics::dx
	{
	struct initializer
		{
		struct implementation;
		initializer();
		~initializer();
		utils::polymorphic_value<implementation> implementation_ptr;
		};
	}

#ifdef utils_implementation
#include "exposed_MS.h"
#include "base.cpp"
#endif