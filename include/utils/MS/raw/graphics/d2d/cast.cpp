#include "cast.h"

namespace utils::MS::raw::graphics::d2d
	{
	D2D1_SIZE_U cast(utils::math::vec2u32 vec) noexcept { return {vec.x(), vec.y()}; }
	D2D1_POINT_2F cast(utils::math::vec2f vec) noexcept { return {vec.x(), vec.y()}; }
	utils::math::vec2f cast(D2D1_POINT_2F vec) noexcept { return {vec.x  , vec.y  }; }

	D2D1_COLOR_F cast(const utils::graphics::colour::rgba_f& colour) noexcept
		{
		return
			{
			.r{colour.r()},
			.g{colour.g()},
			.b{colour.b()},
			.a{colour.a()},
			};
		}
	}