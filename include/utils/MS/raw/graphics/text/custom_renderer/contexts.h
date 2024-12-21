#pragma once

#include "../../d2d/context.h"
#include "common.h"

#include <optional>
#include <functional>

#include <utils/memory.h>
#include <utils/math/vec2.h>
#include <utils/math/geometry/shape/ab.h>
#include <utils/math/geometry/shape/mixed.h>

namespace utils::MS::raw::graphics::text::custom_renderer
	{
	struct contexts
		{
		d2d::context::com_ptr render_context;
		std::vector<shape_outline_t> outlines;
		std::vector<shape_segment_t> strikethroughs;
		std::vector<shape_segment_t> underlines;
		std::vector<winrt::com_ptr<ID2D1TransformedGeometry>> dx_geometries;
		};
	}