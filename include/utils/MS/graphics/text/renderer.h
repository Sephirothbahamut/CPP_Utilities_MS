#pragma once

#include <string>
#include <vector>
#include <memory>

#include <utils/matrix.h>
#include <utils/alignment.h>
#include <utils/graphics/colour.h>
#include <utils/math/vec.h>
#include <utils/math/rect.h>
#include <utils/math/geometry/shape/ab.h>
#include <utils/math/geometry/shape/mixed.h>
#include <utils/polymorphic_value.h>

#include "format.h"
#include "properties.h"
#include "formatted_string.h"
#include "../dx/initializer.h"

namespace utils::MS::graphics::text
	{
	using shape_outline= utils::math::geometry::shape::mixed<utils::math::geometry::ends::closeable::create::closed()>;
	using shape_segment = utils::math::geometry::shape::segment;

	struct output
		{
		utils::matrix<utils::graphics::colour::rgba_f> image;
		std::vector<shape_outline> outlines      ;
		std::vector<shape_segment> strikethroughs;
		std::vector<shape_segment> underlines    ;
		};

	/// <summary> Only exists to workaround my inability to make polyline.side work properly with bad fonts that have self intersecting curves. </summary>
	class dx_geometry_output
		{
		struct implementation;

		public:
			dx_geometry_output();
			~dx_geometry_output();
			bool is_inside(const utils::math::vec2f& coords) const noexcept;

			utils::polymorphic_value<implementation> implementation_ptr;
		};

	class renderer : utils::oop::non_copyable, utils::oop::non_movable
		{
		struct implementation;

		public:
			renderer(dx::initializer& dx_initializer, const utils::math::vec2s& resolution, const utils::graphics::colour::rgba_f& clear_colour = utils::graphics::colour::rgba_f{0.f});
			~renderer();
			
			void clear(const utils::graphics::colour::rgba_f& colour = utils::graphics::colour::rgba_f{0.f});
			void reset(const utils::math::vec2s& resolution, const utils::graphics::colour::rgba_f& clear_colour = utils::graphics::colour::rgba_f{0.f});
			void draw_text(const format& format, const std::string& string, const utils::math::rect<float>& region);
			void draw_text(const formatted_string& text, const utils::math::vec2f position);

			output get_output() const;

			/// <summary> Only exists to workaround my inability to make polyline.side work properly with bad fonts that have self intersecting curves. </summary>
			dx_geometry_output get_dx_geometry_output() const;
			const region::rendering& get_default_rendering_properties() const noexcept;
			      region::rendering& get_default_rendering_properties()       noexcept;

		utils::polymorphic_value<implementation> implementation_ptr;
		};
	}

#ifdef utils_implementation
#include "renderer.cpp"
#endif