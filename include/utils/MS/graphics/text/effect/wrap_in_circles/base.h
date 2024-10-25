#pragma once

#include <utils/polymorphic_value.h>

#include "../initializer.h"

namespace utils::MS::graphics::dx
	{
	struct context
		{
		struct implementation;
		context(initializer& initializer);
		context();
		~context();
		utils::polymorphic_value<implementation> implementation_ptr;
		};
	}

#ifdef utils_implementation
#include "exposed_MS.h"
#include "implementation.cpp"
#endif