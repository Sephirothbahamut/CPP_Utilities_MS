#pragma once

#include "base.h"

#include <utils/polymorphic_value.h>

#include "../../../raw/graphics/d2d.h"
#include "../../../raw/graphics/d3d.h"
#include "../../../raw/graphics/dxgi.h"
#include "../../../raw/graphics/dw.h"

namespace utils::MS::graphics::dx
	{
	struct context::implementation
		{
		implementation(utils::MS::raw::graphics::d2d::device::com_ptr d2d_device);

		utils::MS::raw::graphics::d2d::context::com_ptr d2d_context{};
		};
	}

#ifdef utils_implementation
#include "exposed_MS.cpp"
#endif