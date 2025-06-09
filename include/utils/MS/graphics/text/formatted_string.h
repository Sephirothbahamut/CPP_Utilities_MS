#pragma once

#include <string>
#include <vector>

#include <utils/math/vec.h>
#include <utils/graphics/colour.h>
#include <utils/polymorphic_value.h>

#include "format.h"
#include "../dx/initializer.h"


namespace utils::MS::graphics::text
	{
	struct formatted_string
		{
		struct create_info
			{
			std::u16string     string;
			format             format;
			/// <summary> If your renderer is DPI-aware, sizes is in DIPs </summary>
			utils::math::vec2f sizes;
			};

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

		formatted_string(const create_info& create_info);
		formatted_string(const std::u16string& string, const text::format& format, const utils::math::vec2f& sizes);
		void reset_properties_regions_to_format() noexcept;

		std::u16string     string;
		format             format;
		/// <summary> If your renderer is DPI-aware, sizes is in DIPs </summary>
		utils::math::vec2f sizes;

		utils::MS::graphics::text::regions::properties::regions properties_regions;
		/// <summary> 
		/// Indices at which you wish to force splitting shapes groups. Useful for applying additonal formatting outside of text rendering.
		/// Generated outline shapes may merge multiple characters of the source string, so if you want to do your own changes to specific regions,
		/// you must make sure that such regions are not in a merged outline with what comes before or after.
		/// Example: I want to make the text's outline squiggly in region [5, 8]
		/// Add a custom split at indices 5 and 8
		/// The rendered outlines will be separate in regions [0, 4], [5, 8] and [9, ...]
		/// Then take the outlines in region [5, 8] and make them squiggly.
		/// </summary>
		std::vector<size_t> custom_splits;

		renderable finalize     (dx::initializer& dx_initializer) const noexcept;
		renderable shrink_to_fit(dx::initializer& dx_initializer, float step = 1.f) noexcept;
		};
	}

#ifdef utils_implementation
#include "formatted_string_exposed_MS.h"
#include "formatted_string.cpp"
#endif