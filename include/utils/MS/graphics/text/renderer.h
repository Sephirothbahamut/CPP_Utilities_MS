#pragma once

#include <string>
#include <vector>
#include <memory>

#include <utils/matrix.h>
#include <utils/alignment.h>
#include <utils/graphics/colour.h>
#include <utils/math/vec2.h>
#include <utils/math/rect.h>
#include <utils/math/geometry/shape/mixed.h>

#include "format.h"
#include "formatted_string.h"

namespace utils::MS::graphics::text
	{
	class renderer : utils::oop::non_copyable, utils::oop::non_movable
		{
		struct implementation;

		public:
			renderer();
			renderer(const utils::math::vec2s& resolution, const utils::graphics::colour::rgba_f& clear_colour = utils::graphics::colour::rgba_f{0.f});
			~renderer();
			
			void clear(const utils::graphics::colour::rgba_f& colour = utils::graphics::colour::rgba_f{0.f});
			void reset(const utils::math::vec2s& resolution, const utils::graphics::colour::rgba_f& clear_colour = utils::graphics::colour::rgba_f{0.f});
			void draw_text(const format& format, const std::string& string, const utils::math::rect<float>& region);
			void draw_text(const formatted_string& text, const utils::math::vec2f position);

			utils::matrix<utils::graphics::colour::rgba_f> get_image() const;

		std::unique_ptr<implementation> implementation_ptr;
		};
	}

#ifdef utils_implementation
#include "renderer.cpp"
#endif