#pragma once

#include <vector>

#include <utils/matrix.h>
#include <utils/graphics/colour.h>
#include <utils/containers/regions.h>
#include <utils/math/geometry/shape/ab.h>
#include <utils/math/geometry/shape/aabb.h>
#include <utils/math/geometry/shape/mixed.h>

namespace utils::MS::graphics::text
	{
	using shape_outline = utils::math::geometry::shape::mixed<utils::math::geometry::ends::closeable::create::closed()>;
	using glyph         = std::vector<shape_outline>;
	using shape_segment = utils::math::geometry::shape::segment;
	using shape_rect    = utils::math::geometry::shape::aabb;


	using output_image = utils::matrix<utils::graphics::colour::rgba_f>;
	struct output_shapes
		{
		template <typename T> 
		struct region_value_pair { utils::containers::region region; T value; };

		std::vector<region_value_pair<std::vector<glyph>>                   > glyphs        ;
		std::vector<region_value_pair<utils::math::geometry::shape::aabb   >> rects         ;
		std::vector<                  utils::math::geometry::shape::segment > strikethroughs;
		std::vector<                  utils::math::geometry::shape::aabb    > underlines    ;

		inline void clear() noexcept
			{
			glyphs        .clear();
			rects         .clear();
			strikethroughs.clear();
			underlines    .clear();
			}
		};
	}