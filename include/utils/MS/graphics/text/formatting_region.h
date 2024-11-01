#pragma once

#include <string>
#include <vector>

#include <utils/math/vec2.h>
#include <utils/graphics/colour.h>

#include "format.h"
#include "effect/base.h"

namespace utils::MS::graphics::text
	{
	struct region { size_t begin{0}; size_t count{1}; };

	struct formatted_string : utils::oop::non_copyable, utils::oop::non_movable
		{
		struct implementation;

		formatted_string();
		formatted_string(const std::string& string, const format& format, const utils::math::vec2f& sizes);
		~formatted_string();

		void add_effect(effect::base& effect, region region);
		void render();

		std::unique_ptr<implementation> implementation_ptr;
		};
	}

#ifdef utils_implementation
#include "formatted_string.cpp"
#endif