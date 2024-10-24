#pragma once

#include <string>
#include <vector>

#include <utils/alignment.h>
#include <utils/graphics/colour.h>

namespace utils::MS::graphics::text
	{
	enum class weight
		{
		thin       = 100,
		light      = 300,
		semi_light = 350,
		normal     = 400,
		semi_bold  = 600,
		bold       = 700,
		};
	enum class style { normal, oblique, italic };
	enum class antialiasing { none, greyscale, cleartype };

	struct format
		{
		std::string font;
		std::vector<std::string> fallback_fonts;

		float size{16.f};
		struct alignment
			{
			utils::alignment::vertical   vertical  {utils::alignment::vertical  ::top };
			utils::alignment::horizontal horizontal{utils::alignment::horizontal::left};
			} alignment;

		weight       weight      {weight      ::normal   };
		style        style       {style       ::normal   };
		antialiasing antialiasing{antialiasing::greyscale};

		std::string locale{"en-gb"};

		utils::graphics::colour::rgba_f colour{utils::graphics::colour::base::black};
		bool shrink_to_fit{false};
		};
	}