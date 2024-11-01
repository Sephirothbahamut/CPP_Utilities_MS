#pragma once

#include <utils/math/vec.h>
#include <utils/math/rect.h>
#include <utils/graphics/colour.h>

#include "common.h"

namespace utils::MS::raw::graphics::d2d
	{
	D2D1_SIZE_U   cast(utils::math::vec2u32 vec) noexcept;
	D2D1_POINT_2F cast(utils::math::vec2f vec) noexcept;
	utils::math::vec2f cast(D2D1_POINT_2F vec) noexcept;

	D2D1_COLOR_F cast(const utils::graphics::colour::rgba_f& colour) noexcept;
	}

#ifdef utils_implementation
#include "cast.cpp"
#endif