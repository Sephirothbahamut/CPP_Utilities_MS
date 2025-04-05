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

#include "output.h"
#include "format.h"
#include "formatted_string.h"
#include "../dx/initializer.h"

namespace utils::MS::graphics::text
	{

	class renderer : utils::oop::non_copyable, utils::oop::non_movable
		{
		struct implementation;

		public:
			struct create_info
				{
				/// <summary> Output resolution in pixels </summary>
				utils::math::vec2s resolution;
				utils::graphics::colour::rgba_f clear_colour{0.f};
				utils::math::vec2f dpi{96.f, 96.f};
				};

			renderer(dx::initializer& dx_initializer, const create_info& create_info);
			~renderer();
			
			void clear(const utils::graphics::colour::rgba_f& colour = utils::graphics::colour::rgba_f{0.f});
			void reset(const create_info& create_info);

			/// <summary> Region is in dips (see utils::MS::graphics::conversions) </summary>
			void draw_text(const format& format, const std::string& string, const utils::math::rect<float>& region);

			/// <summary> Position is in dips (see utils::MS::graphics::conversions) </summary>
			void draw_text(const formatted_string::renderable& text, const utils::math::vec2f& position);

			/// <summary> Output geometries are in dips (see utils::MS::graphics::conversions) </summary>
			utils::MS::graphics::text::output_shapes get_output_shapes() const;
			utils::MS::graphics::text::output_image  get_output_image () const;

			const utils::MS::graphics::text::regions::properties& get_default_rendering_properties() const noexcept;
			      utils::MS::graphics::text::regions::properties& get_default_rendering_properties()       noexcept;

		utils::polymorphic_value<implementation> implementation_ptr;
		};
	}

#ifdef utils_implementation
#include "renderer.cpp"
#endif