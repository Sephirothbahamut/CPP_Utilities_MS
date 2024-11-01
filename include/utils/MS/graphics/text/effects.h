#pragma once

#include <string>
#include <vector>

#include <utils/alignment.h>
#include <optional>
#include <utils/graphics/colour.h>

#include <utils/containers/regions.h>

namespace utils::MS::graphics::text
	{
	struct effects_opt
		{
		std::optional<bool                           > text_to_image       {std::nullopt};
		std::optional<utils::graphics::colour::rgba_f> text_colour         {std::nullopt};
		std::optional<bool                           > outline_to_image    {std::nullopt};
		std::optional<bool                           > outline_to_shapes   {std::nullopt};
		std::optional<utils::graphics::colour::rgba_f> outline_colour      {std::nullopt};
		std::optional<bool                           > decorators_to_image {std::nullopt};
		std::optional<bool                           > decorators_to_shapes{std::nullopt};
		std::optional<utils::graphics::colour::rgba_f> decorators_colour   {std::nullopt};

		bool operator==(const effects_opt& other) const noexcept;
		};

	struct effects
		{
		bool                            text_to_image       {true              };
		utils::graphics::colour::rgba_f text_colour         {0.f, 0.f, 0.f, 1.f};
		bool                            outline_to_image    {false             };
		bool                            outline_to_shapes   {false             };
		utils::graphics::colour::rgba_f outline_colour      {0.f, 0.f, 0.f, 1.f};
		bool                            decorators_to_image {true              };
		bool                            decorators_to_shapes{false             };
		utils::graphics::colour::rgba_f decorators_colour   {0.f, 0.f, 0.f, 1.f};

		effects apply_opt(const effects_opt& optional) const noexcept;
		};

	struct effects_regions
		{
		utils::containers::regions<bool                           > text_to_image       ;
		utils::containers::regions<utils::graphics::colour::rgba_f> text_colour         ;
		utils::containers::regions<bool                           > outline_to_image    ;
		utils::containers::regions<bool                           > outline_to_shapes   ;
		utils::containers::regions<utils::graphics::colour::rgba_f> outline_colour      ;
		utils::containers::regions<bool                           > decorators_to_image ;
		utils::containers::regions<bool                           > decorators_to_shapes;
		utils::containers::regions<utils::graphics::colour::rgba_f> decorators_colour   ;

		utils::containers::regions<effects_opt> evaluate_effects_regions() const noexcept;
		};
	}

#ifdef utils_implementation
#include "effects.cpp"
#endif