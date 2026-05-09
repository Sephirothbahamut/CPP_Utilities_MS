#include "cast.h"

namespace utils::MS::raw::graphics::d2d
	{
	utils::math::vec2f cast(D2D1_POINT_2F vec) noexcept { return {vec.x    , vec.y     }; }
	utils::math::vec2s cast(D2D1_SIZE_U   vec) noexcept { return {vec.width, vec.height}; }

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

	utils::math::rect<float   > cast(const D2D1_RECT_F                & rect) noexcept { return {      rect.left ,      rect.top  ,        rect.right ,         rect.bottom }; }
	utils::math::rect<long    > cast(const D2D1_RECT_L                & rect) noexcept { return {      rect.left ,      rect.top  ,        rect.right ,         rect.bottom }; }
	utils::math::rect<unsigned> cast(const D2D1_RECT_U                & rect) noexcept { return {      rect.left ,      rect.top  ,        rect.right ,         rect.bottom }; }
	D2D1_RECT_F                 cast(const utils::math::rect<float   >& rect) noexcept { return {.left{rect.ll()}, .top{rect.up()}, .right{rect.rr() }, .bottom{rect.dw()  }}; }
	D2D1_RECT_L                 cast(const utils::math::rect<long    >& rect) noexcept { return {.left{rect.ll()}, .top{rect.up()}, .right{rect.rr() }, .bottom{rect.dw()  }}; }
	D2D1_RECT_U                 cast(const utils::math::rect<unsigned>& rect) noexcept { return {.left{rect.ll()}, .top{rect.up()}, .right{rect.rr() }, .bottom{rect.dw()  }}; }

	utils::math::vec2f cast(const D2D1_VECTOR_2F    & vec) noexcept { return {vec.x  , vec.y           }; }
	D2D1_VECTOR_2F     cast(const utils::math::vec2f& vec) noexcept { return {vec.x(), vec.y()         }; }
	utils::math::vec3f cast(const D2D1_VECTOR_3F    & vec) noexcept { return {vec.x  , vec.y  , vec.z  }; }
	D2D1_VECTOR_3F     cast(const utils::math::vec3f& vec) noexcept { return {vec.x(), vec.y(), vec.z()}; }
	}