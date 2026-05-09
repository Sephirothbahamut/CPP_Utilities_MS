#pragma once

#include "windows.h"
#include <dcomp.h>
#include <d2d1_3.h>
#include <d3d11_3.h>
#include <dwrite_3.h>

#include <utils/math/rect.h>
#include <utils/math/vec.h>
#include <utils/math/vec.h>

namespace utils::MS
	{
	inline utils::math::rect<long> cast(const RECT& rect) noexcept { return {      rect.left ,      rect.top  ,        rect.right ,         rect.bottom }; }
	inline RECT cast(const utils::math::rect<long>& rect) noexcept { return {.left{rect.ll()}, .top{rect.up()}, .right{rect.rr() }, .bottom{rect.dw()  }}; }
	}