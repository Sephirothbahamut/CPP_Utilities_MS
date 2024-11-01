#pragma once

#include <string>
#include <vector>

#include <utils/math/vec2.h>
#include <utils/graphics/colour.h>
#include <utils/polymorphic_value.h>

#include "format.h"
#include "properties.h"
#include "../dx/initializer.h"

namespace utils::MS::graphics::text
	{
	struct formatted_string : utils::oop::non_copyable, utils::oop::non_movable
		{
		struct implementation;

		formatted_string(dx::initializer& dx_initializer);
		formatted_string(dx::initializer& dx_initializer, const std::string& string, const format& format, const utils::math::vec2f& sizes);
		~formatted_string();

		void apply_properties(const utils::containers::regions<region::properties>& regions_properties);
		void shrink_to_fit();

		utils::polymorphic_value<implementation> implementation_ptr;
		};
	}

#ifdef utils_implementation
#include "formatted_string_exposed_MS.h"
#include "formatted_string.cpp"
#endif