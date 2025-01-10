#pragma once

#include <utils/math/geometry/shape/ab.h>
#include <utils/math/geometry/shape/mixed.h>

namespace utils::MS::raw::graphics::text::custom_renderer
	{
	using shape_outline_t = utils::math::geometry::shape::mixed<utils::math::geometry::ends::closeable::create::closed()>;
	using shape_segment_t = utils::math::geometry::shape::segment;

	using glyph_t = std::vector<shape_outline_t>;
	}