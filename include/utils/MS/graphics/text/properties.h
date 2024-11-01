#pragma once

#include <string>
#include <vector>

#include <utils/alignment.h>
#include <optional>
#include <utils/graphics/colour.h>

#include <utils/containers/regions.h>

#include "format.h"

namespace utils::MS::graphics::text::region
	{
	struct formatting_regions
		{
		utils::containers::regions<std::string > font  ;
		utils::containers::regions<float       > size  ;
		utils::containers::regions<text::weight> weight;
		utils::containers::regions<text::style > style ;
		utils::containers::regions<std::string > locale;
		};

	struct formatting_opt
		{
		std::optional<std::string > font  {std::nullopt};
		std::optional<float       > size  {std::nullopt};
		std::optional<text::weight> weight{std::nullopt};
		std::optional<text::style > style {std::nullopt};
		std::optional<std::string > locale{std::nullopt};

		bool operator==(const formatting_opt& other) const noexcept = default;
		};
	
	struct rendering_element_opt
		{
		std::optional<bool                           > to_image{std::nullopt};
		std::optional<utils::graphics::colour::rgba_f> colour  {std::nullopt};

		bool operator==(const rendering_element_opt& other) const noexcept = default;
		};
	struct rendering_shapes_element_opt
		{
		std::optional<bool                           > to_image {std::nullopt};
		std::optional<bool                           > to_shapes{std::nullopt};
		std::optional<utils::graphics::colour::rgba_f> colour   {std::nullopt};

		bool operator==(const rendering_shapes_element_opt& other) const noexcept = default;
		};
	struct rendering_opt
		{
		rendering_element_opt        text      ;
		rendering_shapes_element_opt outline   ;
		rendering_shapes_element_opt decorators;

		bool operator==(const rendering_opt& other) const noexcept = default;
		};
	
	struct rendering_element
		{
		bool                            to_image;
		utils::graphics::colour::rgba_f colour  ;

		rendering_element apply_opt(const rendering_element_opt& optional) const noexcept;
		};

	struct rendering_element_regions
		{
		utils::containers::regions<bool                           > to_image;
		utils::containers::regions<utils::graphics::colour::rgba_f> colour  ;
		};

	struct rendering_shapes_element
		{
		bool                            to_image ;
		bool                            to_shapes;
		utils::graphics::colour::rgba_f colour   ;

		rendering_shapes_element apply_opt(const rendering_shapes_element_opt& optional) const noexcept;
		};

	struct rendering_shapes_element_regions
		{
		utils::containers::regions<bool                           > to_image ;
		utils::containers::regions<bool                           > to_shapes;
		utils::containers::regions<utils::graphics::colour::rgba_f> colour   ;
		};

	struct rendering
		{
		rendering_element        text      {.to_image{true },                    .colour{0.f, 0.f, 0.f, 1.f}};
		rendering_shapes_element outline   {.to_image{false}, .to_shapes{false}, .colour{0.f, 0.f, 0.f, 1.f}};
		rendering_shapes_element decorators{.to_image{true }, .to_shapes{false}, .colour{0.f, 0.f, 0.f, 1.f}};

		rendering apply_opt(const rendering_opt& optional) const noexcept;
		};
	
	struct rendering_regions
		{
		rendering_element_regions        text      ;
		rendering_shapes_element_regions outline   ;
		rendering_shapes_element_regions decorators;
		};

	struct properties
		{
		formatting_opt formatting;
		rendering_opt  rendering ;

		bool operator==(const properties& other) const noexcept = default;
		};

	struct properties_regions
		{
		formatting_regions formatting;
		rendering_regions  rendering ;

		utils::containers::regions<properties> evaluate_region_properties() const noexcept;
		};
	}
namespace utils::MS::graphics::text
	{
	using properties_regions = region::properties_regions;
	}

#ifdef utils_implementation
#include "properties.cpp"
#endif