#pragma once

#include <string>
#include <vector>

#include <utils/math/vec.h>
#include <utils/graphics/colour.h>
#include <utils/polymorphic_value.h>

#include "format.h"
#include "properties.h"
#include "../dx/initializer.h"

namespace utils::MS::graphics::text
	{
	struct formatted_string
		{
		class renderable : utils::oop::non_copyable, utils::oop::non_movable
			{
			friend struct utils::MS::graphics::text::formatted_string;
			public:
				struct implementation;
				utils::polymorphic_value<implementation> implementation_ptr;
				~renderable();

			private:
				renderable(dx::initializer& dx_initializer, const formatted_string& formatted_string);
				renderable(dx::initializer& dx_initializer, formatted_string& formatted_string, float step);
			};

		std::string        string;
		format             format;
		/// <summary> 
		/// If your renderer is DPI-aware, sizes is in DIPs
		/// </summary>
		utils::math::vec2f sizes;

		utils::MS::graphics::text::properties_regions properties_regions;

		renderable finalize     (dx::initializer& dx_initializer) const noexcept;
		renderable shrink_to_fit(dx::initializer& dx_initializer, float step = 1.f) noexcept;
		};
	}

#ifdef utils_implementation
#include "formatted_string_exposed_MS.h"
#include "formatted_string.cpp"
#endif