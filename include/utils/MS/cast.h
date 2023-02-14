#pragma once

#include <Windows.h>
#include <dcomp.h>
#include <d2d1_3.h>
#include <d3d11_3.h>
#include <dwrite_3.h>

#include <utils/math/rect.h>
#include <utils/math/vec2.h>
#include <utils/math/vec3.h>

namespace utils::MS
	{
	inline utils::math::rect<long> cast(const RECT& rect) noexcept { return {.ll  {rect.left}, .up {rect.top}, .rr   {rect.right}, .dw    {rect.bottom}}; }
	inline RECT cast(const utils::math::rect<long>& rect) noexcept { return {.left{rect.left}, .top{rect.top}, .right{rect.right}, .bottom{rect.bottom}}; }

	namespace graphics::d2d
		{
		inline utils::math::rect<float   > cast(const D2D1_RECT_F                & rect) noexcept { return {.ll  {rect.left}, .up {rect.top}, .rr   {rect.right}, .dw    {rect.bottom}}; }
		inline D2D1_RECT_F                 cast(const utils::math::rect<float   >& rect) noexcept { return {.left{rect.left}, .top{rect.top}, .right{rect.right}, .bottom{rect.bottom}}; }
		inline utils::math::rect<long    > cast(const D2D1_RECT_L                & rect) noexcept { return {.ll  {rect.left}, .up {rect.top}, .rr   {rect.right}, .dw    {rect.bottom}}; }
		inline D2D1_RECT_L                 cast(const utils::math::rect<long    >& rect) noexcept { return {.left{rect.left}, .top{rect.top}, .right{rect.right}, .bottom{rect.bottom}}; }
		inline utils::math::rect<unsigned> cast(const D2D1_RECT_U                & rect) noexcept { return {.ll  {rect.left}, .up {rect.top}, .rr   {rect.right}, .dw    {rect.bottom}}; }
		inline D2D1_RECT_U                 cast(const utils::math::rect<unsigned>& rect) noexcept { return {.left{rect.left}, .top{rect.top}, .right{rect.right}, .bottom{rect.bottom}}; }

		inline utils::math::vec2f cast(const D2D1_VECTOR_2F    & vec) noexcept { return {vec.x, vec.y}; }
		inline D2D1_VECTOR_2F     cast(const utils::math::vec2f& vec) noexcept { return {vec.x, vec.y}; }
		inline utils::math::vec3f cast(const D2D1_VECTOR_3F    & vec) noexcept { return {vec.x, vec.y, vec.z}; }
		inline D2D1_VECTOR_3F     cast(const utils::math::vec3f& vec) noexcept { return {vec.x, vec.y, vec.z}; }
		}
	}