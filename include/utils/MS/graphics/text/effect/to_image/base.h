#pragma once

#include "../base.h"

#include <utils/matrix.h>
#include <utils/graphics/colour.h>
#include <utils/polymorphic_value.h>

#include "../../../dx/initializer.h"

namespace utils::MS::graphics::text::effect
	{
	struct to_image : base
		{
		struct implementation;

		to_image(
			utils::MS::graphics::dx::initializer & initializer ,
			const utils::math::vec2s             & resolution  ,
			const utils::graphics::colour::rgba_f& clear_colour,
			const utils::graphics::colour::rgba_f& text_colour );
		~to_image();

		utils::matrix<utils::graphics::colour::rgba_f> get_image();
		};
	}

#ifdef utils_implementation
#include "exposed_MS.h"
#include "base.cpp"
#endif