#pragma once

#include <utils/polymorphic_value.h>

#include "../initializer.h"

namespace utils::MS::graphics::dx
	{
	struct context
		{
		struct implementation;
		context(initializer& initializer);
		~context();
		utils::polymorphic_value<implementation> implementation_ptr;
		};
	}

#ifdef utils_implementation
#include "exposed_MS.h"
#include "base.cpp"
#endif