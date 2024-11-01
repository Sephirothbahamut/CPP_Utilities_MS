#pragma once

#include "properties.h"

namespace utils::MS::graphics::text::region
	{

	rendering_element rendering_element::apply_opt(const rendering_element_opt& optional) const noexcept
		{
		const rendering_element ret
			{
			.to_image{optional.to_image.value_or(to_image)},
			.colour  {optional.colour  .value_or(colour  )}
			};
		return ret;
		}

	rendering_shapes_element rendering_shapes_element::apply_opt(const rendering_shapes_element_opt& optional) const noexcept
		{
		const rendering_shapes_element ret
			{
			.to_image {optional.to_image .value_or(to_image )},
			.to_shapes{optional.to_shapes.value_or(to_shapes)},
			.colour   {optional.colour   .value_or(colour   )}
			};
		return ret;
		}

	rendering rendering::apply_opt(const rendering_opt& optional) const noexcept
		{
		const rendering ret
			{
			.text      {text      .apply_opt(optional.text      )},
			.outline   {outline   .apply_opt(optional.outline   )},
			.decorators{decorators.apply_opt(optional.decorators)},
			};
		return ret;
		}


	utils::containers::regions<properties> properties_regions::evaluate_region_properties() const noexcept
		{
		utils::containers::regions<properties> ret;

		// formatting.font               ;
		// formatting.size               ;
		// formatting.style              ;
		// formatting.weight             ;
		// rendering.text.to_image       ;
		// rendering.text.colour         ;
		// rendering.outline.to_image    ;
		// rendering.outline.to_shapes   ;
		// rendering.outline.colour      ;
		// rendering.decorators.to_image ;
		// rendering.decorators.to_shapes;
		// rendering.decorators.colour   ;

		size_t element_index{0};
		size_t end{std::max(
			{
			formatting.font               .values_size(),
			formatting.size               .values_size(),
			formatting.weight             .values_size(),
			formatting.style              .values_size(),
			formatting.locale             .values_size(),
			rendering.text.to_image       .values_size(),
			rendering.text.colour         .values_size(),
			rendering.outline.to_image    .values_size(),
			rendering.outline.to_shapes   .values_size(),
			rendering.outline.colour      .values_size(),
			rendering.decorators.to_image .values_size(),
			rendering.decorators.to_shapes.values_size(),
			rendering.decorators.colour   .values_size(),
			})};

		while (element_index != end)
			{
			const auto value_formatting_font               {formatting.font               .value_at(element_index)};
			const auto value_formatting_size               {formatting.size               .value_at(element_index)};
			const auto value_formatting_weight             {formatting.weight             .value_at(element_index)};
			const auto value_formatting_style              {formatting.style              .value_at(element_index)};
			const auto value_formatting_locale             {formatting.locale             .value_at(element_index)};
			const auto value_rendering_text_to_image       {rendering.text.to_image       .value_at(element_index)};
			const auto value_rendering_text_colour         {rendering.text.colour         .value_at(element_index)};
			const auto value_rendering_outline_to_image    {rendering.outline.to_image    .value_at(element_index)};
			const auto value_rendering_outline_to_shapes   {rendering.outline.to_shapes   .value_at(element_index)};
			const auto value_rendering_outline_colour      {rendering.outline.colour      .value_at(element_index)};
			const auto value_rendering_decorators_to_image {rendering.decorators.to_image .value_at(element_index)};
			const auto value_rendering_decorators_to_shapes{rendering.decorators.to_shapes.value_at(element_index)};
			const auto value_rendering_decorators_colour   {rendering.decorators.colour   .value_at(element_index)};
			
			const size_t region_begin{element_index};
			const size_t region_end{std::min(
				{
				value_formatting_font               .region.end(),
				value_formatting_size               .region.end(),
				value_formatting_weight             .region.end(),
				value_formatting_style              .region.end(),
				value_formatting_locale             .region.end(),
				value_rendering_text_to_image       .region.end(),
				value_rendering_text_colour         .region.end(),
				value_rendering_outline_to_image    .region.end(),
				value_rendering_outline_to_shapes   .region.end(),
				value_rendering_outline_colour      .region.end(),
				value_rendering_decorators_to_image .region.end(),
				value_rendering_decorators_to_shapes.region.end(),
				value_rendering_decorators_colour   .region.end(),
				})};

			const bool fully_nullopt
				{
				(!value_formatting_font               .value_opt_ref.has_value()) &&
				(!value_formatting_size               .value_opt_ref.has_value()) &&
				(!value_formatting_weight             .value_opt_ref.has_value()) &&
				(!value_formatting_style              .value_opt_ref.has_value()) &&
				(!value_formatting_locale             .value_opt_ref.has_value()) &&
				(!value_rendering_text_to_image       .value_opt_ref.has_value()) &&
				(!value_rendering_text_colour         .value_opt_ref.has_value()) &&
				(!value_rendering_outline_to_image    .value_opt_ref.has_value()) &&
				(!value_rendering_outline_to_shapes   .value_opt_ref.has_value()) &&
				(!value_rendering_outline_colour      .value_opt_ref.has_value()) &&
				(!value_rendering_decorators_to_image .value_opt_ref.has_value()) &&
				(!value_rendering_decorators_to_shapes.value_opt_ref.has_value()) &&
				(!value_rendering_decorators_colour   .value_opt_ref.has_value())
				};
		
			if(!fully_nullopt)
				{
				const properties new_properties
					{
					.formatting
						{
						.font  {value_formatting_font  .value_opt_ref},
						.size  {value_formatting_size  .value_opt_ref},
						.weight{value_formatting_weight.value_opt_ref},
						.style {value_formatting_style .value_opt_ref},
						.locale{value_formatting_locale.value_opt_ref},
						},
					.rendering
						{
						.text
							{
							.to_image{value_rendering_text_to_image.value_opt_ref},
							.colour  {value_rendering_text_colour  .value_opt_ref},
							},
						.outline
							{
							.to_image {value_rendering_outline_to_image .value_opt_ref},
							.to_shapes{value_rendering_outline_to_shapes.value_opt_ref},
							.colour   {value_rendering_outline_colour   .value_opt_ref},
							},
						.decorators
							{
							.to_image {value_rendering_decorators_to_image .value_opt_ref},
							.to_shapes{value_rendering_decorators_to_shapes.value_opt_ref},
							.colour   {value_rendering_decorators_colour   .value_opt_ref},
							}
						}
					};
		
				ret.add(new_properties, {region_begin, region_end - region_begin});
				}

			element_index = region_end;
			}
		return ret;
		}
	}