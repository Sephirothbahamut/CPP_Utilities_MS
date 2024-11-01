#pragma once

#include "effects.h"

namespace utils::MS::graphics::text
	{
	effects effects::apply_opt(const effects_opt& optional) const noexcept
		{
		const effects ret
			{
			.text_to_image       {optional.text_to_image       .value_or(text_to_image       )},
			.text_colour         {optional.text_colour         .value_or(text_colour         )},
			.outline_to_image    {optional.outline_to_image    .value_or(outline_to_image    )},
			.outline_to_shapes   {optional.outline_to_shapes   .value_or(outline_to_shapes   )},
			.outline_colour      {optional.outline_colour      .value_or(outline_colour      )},
			.decorators_to_image {optional.decorators_to_image .value_or(decorators_to_image )},
			.decorators_to_shapes{optional.decorators_to_shapes.value_or(decorators_to_shapes)},
			.decorators_colour   {optional.decorators_colour   .value_or(decorators_colour   )},
			};
		return ret;
		}

	bool effects_opt::operator==(const effects_opt& other) const noexcept
		{
		return
			(
			((text_to_image       .has_value() && other.text_to_image       .has_value() && text_to_image       .value() == other.text_to_image       .value()) || (!text_to_image       .has_value() && !other.text_to_image       .has_value())) &&
			((text_colour         .has_value() && other.text_colour         .has_value() && text_colour         .value() == other.text_colour         .value()) || (!text_colour         .has_value() && !other.text_colour         .has_value())) &&
			((outline_to_image    .has_value() && other.outline_to_image    .has_value() && outline_to_image    .value() == other.outline_to_image    .value()) || (!outline_to_image    .has_value() && !other.outline_to_image    .has_value())) &&
			((outline_to_shapes   .has_value() && other.outline_to_shapes   .has_value() && outline_to_shapes   .value() == other.outline_to_shapes   .value()) || (!outline_to_shapes   .has_value() && !other.outline_to_shapes   .has_value())) &&
			((outline_colour      .has_value() && other.outline_colour      .has_value() && outline_colour      .value() == other.outline_colour      .value()) || (!outline_colour      .has_value() && !other.outline_colour      .has_value())) &&
			((decorators_to_image .has_value() && other.decorators_to_image .has_value() && decorators_to_image .value() == other.decorators_to_image .value()) || (!decorators_to_image .has_value() && !other.decorators_to_image .has_value())) &&
			((decorators_to_shapes.has_value() && other.decorators_to_shapes.has_value() && decorators_to_shapes.value() == other.decorators_to_shapes.value()) || (!decorators_to_shapes.has_value() && !other.decorators_to_shapes.has_value())) &&
			((decorators_colour   .has_value() && other.decorators_colour   .has_value() && decorators_colour   .value() == other.decorators_colour   .value()) || (!decorators_colour   .has_value() && !other.decorators_colour   .has_value()))
			);
		}


	template <typename T>
	struct tmpstrct
		{
		using value_type = T;
		using regions_t = utils::containers::regions<T>;
		using slot_t = regions_t::read_const_slot;

		tmpstrct(const regions_t& regions) : regions{regions}
			{
			set_index(0);
			}


		const regions_t& regions;

		size_t index{0};
		utils::containers::region current_region;
		std::optional<value_type> current_value;

		bool is_valid() { return index < regions.slots_size(); }

		void next() noexcept { if (is_valid()) { set_index(index + 1); } }

		void set_index(size_t index) noexcept
			{
			this->index = index;
			if (is_valid())
				{
				const auto current_slot{regions.slot_at(index)};
				current_region = current_slot.region;
				if (current_slot.value_opt_ref.has_value())
					{
					current_value = current_slot.value_opt_ref.value().get();
					}
				else
					{
					current_value = std::nullopt;
					}
				}
			else
				{
				current_value = std::nullopt;
				current_region.begin = std::numeric_limits<size_t>::max();
				current_region.count = 0;
				}
			}
		};

	//TODO generalize in utils library for any aggregate of any regions
	utils::containers::regions<effects_opt> effects_regions::evaluate_effects_regions() const noexcept
		{
		utils::containers::regions<effects_opt> ret;

		size_t element_index{0};
		size_t end{std::max(
			{
			text_to_image       .values_size(),
			text_colour         .values_size(),
			outline_to_image    .values_size(),
			outline_to_shapes   .values_size(),
			outline_colour      .values_size(),
			decorators_to_image .values_size(),
			decorators_to_shapes.values_size(),
			decorators_colour   .values_size(),
			})};

		while (element_index != end)
			{
			const auto value_opt_text_to_image       {text_to_image       .value_at(element_index)};
			const auto value_opt_text_colour         {text_colour         .value_at(element_index)};
			const auto value_opt_outline_to_image    {outline_to_image    .value_at(element_index)};
			const auto value_opt_outline_to_shapes   {outline_to_shapes   .value_at(element_index)};
			const auto value_opt_outline_colour      {outline_colour      .value_at(element_index)};
			const auto value_opt_decorators_to_image {decorators_to_image .value_at(element_index)};
			const auto value_opt_decorators_to_shapes{decorators_to_shapes.value_at(element_index)};
			const auto value_opt_decorators_colour   {decorators_colour   .value_at(element_index)};
			
			const size_t region_begin{element_index};
			const size_t region_end{std::min(
				{
				value_opt_text_to_image       .region.end(),
				value_opt_text_colour         .region.end(),
				value_opt_outline_to_image    .region.end(),
				value_opt_outline_to_shapes   .region.end(),
				value_opt_outline_colour      .region.end(),
				value_opt_decorators_to_image .region.end(),
				value_opt_decorators_to_shapes.region.end(),
				value_opt_decorators_colour   .region.end(),
				})};

			const bool fully_nullopt
				{
				(!value_opt_text_to_image       .value_opt_ref.has_value()) &&
				(!value_opt_text_colour         .value_opt_ref.has_value()) &&
				(!value_opt_outline_to_image    .value_opt_ref.has_value()) &&
				(!value_opt_outline_to_shapes   .value_opt_ref.has_value()) &&
				(!value_opt_outline_colour      .value_opt_ref.has_value()) &&
				(!value_opt_decorators_to_image .value_opt_ref.has_value()) &&
				(!value_opt_decorators_to_shapes.value_opt_ref.has_value()) &&
				(!value_opt_decorators_colour   .value_opt_ref.has_value())
				};
		
			if(!fully_nullopt)
				{
				const effects_opt new_effects_opt
					{
					.text_to_image       {value_opt_text_to_image       .value_opt_ref},
					.text_colour         {value_opt_text_colour         .value_opt_ref},
					.outline_to_image    {value_opt_outline_to_image    .value_opt_ref},
					.outline_to_shapes   {value_opt_outline_to_shapes   .value_opt_ref},
					.outline_colour      {value_opt_outline_colour      .value_opt_ref},
					.decorators_to_image {value_opt_decorators_to_image .value_opt_ref},
					.decorators_to_shapes{value_opt_decorators_to_shapes.value_opt_ref},
					.decorators_colour   {value_opt_decorators_colour   .value_opt_ref},
					};
		
				ret.add(new_effects_opt, {region_begin, region_end - region_begin});
				}

			element_index = region_end;
			}
		return ret;

		

		//tmpstrct tmp_text_to_image       {text_to_image       };
		//tmpstrct tmp_text_colour         {text_colour         };
		//tmpstrct tmp_outline_to_image    {outline_to_image    };
		//tmpstrct tmp_outline_to_shapes   {outline_to_shapes   };
		//tmpstrct tmp_outline_colour      {outline_colour      };
		//tmpstrct tmp_decorators_to_image {decorators_to_image };
		//tmpstrct tmp_decorators_to_shapes{decorators_to_shapes};
		//tmpstrct tmp_decorators_colour   {decorators_colour   };
		//while (
		//	tmp_text_to_image       .is_valid() ||
		//	tmp_text_colour         .is_valid() ||
		//	tmp_outline_to_image    .is_valid() ||
		//	tmp_outline_to_shapes   .is_valid() ||
		//	tmp_outline_colour      .is_valid() ||
		//	tmp_decorators_to_image .is_valid() ||
		//	tmp_decorators_to_shapes.is_valid() ||
		//	tmp_decorators_colour   .is_valid())
		//	{
		//	size_t region_begin{std::numeric_limits<size_t>::max()};
		//	size_t region_end{0};
		//
		//	if(tmp_text_to_image       .is_valid()) { region_begin = std::min(region_begin, tmp_text_to_image       .current_region.begin); region_end = std::max(region_end, tmp_text_to_image       .current_region.end()); }
		//	if(tmp_text_colour         .is_valid()) { region_begin = std::min(region_begin, tmp_text_colour         .current_region.begin); region_end = std::max(region_end, tmp_text_colour         .current_region.end()); }
		//	if(tmp_outline_to_image    .is_valid()) { region_begin = std::min(region_begin, tmp_outline_to_image    .current_region.begin); region_end = std::max(region_end, tmp_outline_to_image    .current_region.end()); }
		//	if(tmp_outline_to_shapes   .is_valid()) { region_begin = std::min(region_begin, tmp_outline_to_shapes   .current_region.begin); region_end = std::max(region_end, tmp_outline_to_shapes   .current_region.end()); }
		//	if(tmp_outline_colour      .is_valid()) { region_begin = std::min(region_begin, tmp_outline_colour      .current_region.begin); region_end = std::max(region_end, tmp_outline_colour      .current_region.end()); }
		//	if(tmp_decorators_to_image .is_valid()) { region_begin = std::min(region_begin, tmp_decorators_to_image .current_region.begin); region_end = std::max(region_end, tmp_decorators_to_image .current_region.end()); }
		//	if(tmp_decorators_to_shapes.is_valid()) { region_begin = std::min(region_begin, tmp_decorators_to_shapes.current_region.begin); region_end = std::max(region_end, tmp_decorators_to_shapes.current_region.end()); }
		//	if(tmp_decorators_colour   .is_valid()) { region_begin = std::min(region_begin, tmp_decorators_colour   .current_region.begin); region_end = std::max(region_end, tmp_decorators_colour   .current_region.end()); }
		//
		//
		//	const bool fully_nullopt
		//		{
		//		(!tmp_text_to_image       .current_value.has_value()) &&
		//		(!tmp_text_colour         .current_value.has_value()) &&
		//		(!tmp_outline_to_image    .current_value.has_value()) &&
		//		(!tmp_outline_to_shapes   .current_value.has_value()) &&
		//		(!tmp_outline_colour      .current_value.has_value()) &&
		//		(!tmp_decorators_to_image .current_value.has_value()) &&
		//		(!tmp_decorators_to_shapes.current_value.has_value()) &&
		//		(!tmp_decorators_colour   .current_value.has_value())
		//		};
		//
		//	if(!fully_nullopt)
		//		{
		//		const effects_opt new_effects_opt
		//			{
		//			.text_to_image       {tmp_text_to_image       .current_value},
		//			.text_colour         {tmp_text_colour         .current_value},
		//			.outline_to_image    {tmp_outline_to_image    .current_value},
		//			.outline_to_shapes   {tmp_outline_to_shapes   .current_value},
		//			.outline_colour      {tmp_outline_colour      .current_value},
		//			.decorators_to_image {tmp_decorators_to_image .current_value},
		//			.decorators_to_shapes{tmp_decorators_to_shapes.current_value},
		//			.decorators_colour   {tmp_decorators_colour   .current_value},
		//			};
		//
		//		ret.add(new_effects_opt, {region_begin, region_end - region_begin});
		//		}
		//
		//	tmp_text_to_image       .next();
		//	tmp_text_colour         .next();
		//	tmp_outline_to_image    .next();
		//	tmp_outline_to_shapes   .next();
		//	tmp_outline_colour      .next();
		//	tmp_decorators_to_image .next();
		//	tmp_decorators_to_shapes.next();
		//	tmp_decorators_colour   .next();
		//	}

		return ret;
		}
	}