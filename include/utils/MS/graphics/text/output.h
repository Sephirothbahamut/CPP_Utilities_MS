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
	using glyph = std::vector<shape_outline>;
	using shape_segment = utils::math::geometry::shape::segment;
	using shape_rect    = utils::math::geometry::shape::aabb;


	using output_image = utils::matrix<utils::graphics::colour::rgba_f>;
	struct output
		{
		std::vector<glyph        > glyphs;
		std::vector<shape_segment> strikethroughs;
		std::vector<shape_segment> underlines;
		std::vector<shape_rect   > highlights;
		/// <summary>
		/// indices represent the region in the input string
		/// value_type represent the region of the glyphs array that draws that region of the input string.
		/// </summary>
		utils::containers::regions<utils::containers::region> source_string_regions_of_glyphs_indices;

		inline void clear() noexcept
			{
			glyphs        .clear();
			strikethroughs.clear();
			underlines    .clear();
			source_string_regions_of_glyphs_indices.clear();
			}
		};
	}