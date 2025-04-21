#include "formatted_string_exposed_MS.h"

#include "../dx/initializer/exposed_MS.h"

namespace utils::MS::graphics::text
	{
	DWRITE_TEXT_RANGE cast(const utils::containers::region& region)
		{
		return {utils::math::cast_clamp<UINT32>(region.begin), utils::math::cast_clamp<UINT32>(region.count)};
		}

	formatted_string::renderable::implementation::implementation(dx::initializer& dx_initializer) :
		dw_factory{dx_initializer.implementation_ptr->dw_factory}
		{}

	formatted_string::renderable::renderable(dx::initializer& dx_initializer, const formatted_string& formatted_string) :
		implementation_ptr{utils::make_polymorphic_value<formatted_string::renderable::implementation>(dx_initializer)}
		{
		implementation_ptr->create_layout(formatted_string);
		}
	formatted_string::renderable::renderable(dx::initializer& dx_initializer, formatted_string& formatted_string, float step) :
		implementation_ptr{utils::make_polymorphic_value<formatted_string::renderable::implementation>(dx_initializer)}
		{
		implementation_ptr->create_layout(formatted_string);
		implementation_ptr->shrink_to_fit(formatted_string, step);
		}

	formatted_string::renderable::~renderable() = default;



	void for_each_slot(const auto& regions, auto callback)
		{
		for (size_t i{0}; i < regions.slots_count(); i++)
			{
			const auto slot{regions.slot_at_index_of_slots(i)};
			if (!slot.value_ptr) { continue; }
			const auto& value{*slot.value_ptr};
			const auto dx_region{cast(slot.region)};
			callback(value, dx_region);
			}
		}



	void formatted_string::renderable::implementation::create_layout(const formatted_string& formatted_string)
		{
		auto dw_format{utils::MS::raw::graphics::dw::text_format::create(dw_factory, formatted_string.format)};
		dw_layout = utils::MS::raw::graphics::dw::text_layout::create(dw_factory, dw_format, formatted_string.string, formatted_string.sizes);
		
		for_each_slot(formatted_string.properties_regions.format.font, [&](const std::string& value, DWRITE_TEXT_RANGE region)
			{
			if (value.empty()) { return; }
			dw_layout->SetFontFamilyName(utils::MS::string::utf8_to_wide(value).c_str(), region);
			});
		for_each_slot(formatted_string.properties_regions.format.locale, [&](const std::string& value, DWRITE_TEXT_RANGE region)
			{
			if (value.empty()) { return; }
			dw_layout->SetLocaleName(utils::MS::string::utf8_to_wide(value).c_str(), region);
			});

		for_each_slot(formatted_string.properties_regions.format.size                 , [&](const float                     & value, DWRITE_TEXT_RANGE region) { dw_layout->SetFontSize     (                                   value , region); });
		for_each_slot(formatted_string.properties_regions.format.weight               , [&](const MS::graphics::text::weight& value, DWRITE_TEXT_RANGE region) { dw_layout->SetFontWeight   (utils::MS::raw::graphics::dw::cast(value), region); });
		for_each_slot(formatted_string.properties_regions.format.style                , [&](const MS::graphics::text::style & value, DWRITE_TEXT_RANGE region) { dw_layout->SetFontStyle    (utils::MS::raw::graphics::dw::cast(value), region); });
		for_each_slot(formatted_string.properties_regions.format.strikethrough.enabled, [&](const bool                      & value, DWRITE_TEXT_RANGE region) { dw_layout->SetStrikethrough(                                   value , region); });
		for_each_slot(formatted_string.properties_regions.format.underline    .enabled, [&](const bool                      & value, DWRITE_TEXT_RANGE region) { dw_layout->SetUnderline    (                                   value , region); });


		using aggregate_properties_t = utils::containers::aggregate_regions
			<
			utils::MS::graphics::text::regions::properties,
			utils::MS::graphics::text::regions::properties::optional,
			utils::MS::graphics::text::regions::properties::regions,
			utils::MS::graphics::text::regions::properties::accessors_helper
			>;
		const aggregate_properties_t aggregate_properties{formatted_string.properties_regions};

		const auto split_indices{[&]()
			{
			auto tmp{aggregate_properties.split_indices_set()};
			std::copy(formatted_string.custom_splits.begin(), formatted_string.custom_splits.end(), std::inserter(tmp, tmp.end()));
			return std::vector<size_t>{tmp.begin(), tmp.end()};
			}()};
		if (!split_indices.empty())
			{
			assert(split_indices.size() > 1);
			for (size_t i = 0; i < (split_indices.size() - 1); i++)
				{
				const utils::containers::region split_region{split_indices[i], split_indices[i] + 1};
				const auto dx_region{cast(split_region)};
				const auto effects_com_ptr{utils::MS::raw::graphics::text::custom_renderer::effects::create(formatted_string.properties_regions, split_indices[i])};
				dw_layout->SetDrawingEffect(effects_com_ptr.get(), dx_region);
				}
			}
		}

	void formatted_string::renderable::implementation::shrink_to_fit(formatted_string& formatted_string, float step)
		{
		DWRITE_TEXT_METRICS metrics;
		if (!dw_layout) { create_layout(formatted_string); }
		while (true)
			{
			dw_layout->GetMetrics(&metrics);
			if (metrics.height > metrics.layoutHeight)
				{
				float min_size{formatted_string.format.size};

				for (size_t i{0}; i < formatted_string.properties_regions.format.size.slots_count(); i++)
					{
					const auto slot{formatted_string.properties_regions.format.size.slot_at_index_of_slots(i)};
					if (!slot.value_ptr) { continue; }
					const float& value{*slot.value_ptr};
					min_size = std::min(min_size, value);
					}

				if (min_size > step)
					{
					formatted_string.format.size -= step;

					for (size_t i{0}; i < formatted_string.properties_regions.format.size.slots_count(); i++)
						{
						auto slot{formatted_string.properties_regions.format.size.slot_at_index_of_slots(i)};
						if (!slot.value_ptr) { continue; }
						auto& value{*slot.value_ptr};
						value -= step;
						}

					create_layout(formatted_string);
					}
				else { break; }
				}
			else { break; }
			}
		}

	formatted_string::renderable formatted_string::finalize(dx::initializer& dx_initializer) const noexcept
		{
		return formatted_string::renderable{dx_initializer, *this};
		}
	formatted_string::renderable formatted_string::shrink_to_fit(dx::initializer& dx_initializer, float step) noexcept
		{
		return formatted_string::renderable{dx_initializer, *this, step};
		}
	}
