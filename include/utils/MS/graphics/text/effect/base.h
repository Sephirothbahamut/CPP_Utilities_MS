#pragma once

#include <utils/polymorphic_value.h>

namespace utils::MS::graphics::text::effect
	{
	struct implementation_base;

	struct base
		{
		utils::polymorphic_value<implementation_base> implementation_ptr;
		};
	}