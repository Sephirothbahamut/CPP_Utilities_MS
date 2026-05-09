#pragma once

#include <utils/math/vec.h>
#include <utils/math/rect.h>
#include <utils/graphics/colour.h>

#include "common.h"

namespace utils::MS::raw::graphics::d2d
	{
	template <typename out_2d_type>
	out_2d_type cast(const utils::math::concepts::vec_size<2> auto& vec) noexcept { return {vec.x(), vec.y()}; }
	utils::math::vec2f cast(D2D1_POINT_2F vec) noexcept;
	utils::math::vec2s cast(D2D1_SIZE_U   vec) noexcept;
	D2D1_COLOR_F cast(const utils::graphics::colour::rgba_f& colour) noexcept;


	
	utils::math::rect<float   > cast(const D2D1_RECT_F                & rect) noexcept;
	utils::math::rect<long    > cast(const D2D1_RECT_L                & rect) noexcept;
	utils::math::rect<unsigned> cast(const D2D1_RECT_U                & rect) noexcept;
	D2D1_RECT_F                 cast(const utils::math::rect<float   >& rect) noexcept;
	D2D1_RECT_L                 cast(const utils::math::rect<long    >& rect) noexcept;
	D2D1_RECT_U                 cast(const utils::math::rect<unsigned>& rect) noexcept;

	utils::math::vec2f cast(const D2D1_VECTOR_2F    & vec) noexcept;
	D2D1_VECTOR_2F     cast(const utils::math::vec2f& vec) noexcept;
	utils::math::vec3f cast(const D2D1_VECTOR_3F    & vec) noexcept;
	D2D1_VECTOR_3F     cast(const utils::math::vec3f& vec) noexcept;
	}

#ifdef utils_implementation
#include "cast.cpp"
#endif