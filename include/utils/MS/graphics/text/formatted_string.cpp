#include "formatted_string_exposed_MS.h"

#include "../dx/initializer/exposed_MS.h"

namespace utils::MS::graphics::text
	{
	DWRITE_TEXT_RANGE cast(const utils::containers::region& region)
		{
		return {utils::math::cast_clamp<UINT32>(region.begin), utils::math::cast_clamp<UINT32>(region.count)};
		}

	formatted_string::implementation::implementation(dx::initializer& dx_initializer) :
		dw_factory{dx_initializer.implementation_ptr->dw_factory}
		{
		}

	void formatted_string::implementation::create_layout(const formatted_string& formatted_string)
		{
		auto dw_format{utils::MS::raw::graphics::dw::text_format::create(dw_factory, formatted_string.format)};
		dw_layout = utils::MS::raw::graphics::dw::text_layout::create(dw_factory, dw_format, formatted_string.string, formatted_string.sizes);


		const auto properties_regions{formatted_string.properties_regions.evaluate_region_properties()};

		for (size_t i{0}; i < properties_regions.slots_size(); i++)
			{
			const auto& slot{properties_regions.slot_at(i)};
			if (!slot.value_opt_ref) { continue; }
			const auto& properties{slot.value_opt_ref.value().get()};

			const auto dx_region{cast(slot.region)};
				
			if (properties.formatting.font  .has_value()) { dw_layout->SetFontFamilyName(utils::MS::string::utf8_to_wide   (properties.formatting.font  .value()).c_str(), dx_region); }
			if (properties.formatting.size  .has_value()) { dw_layout->SetFontSize      (                                   properties.formatting.size  .value()         , dx_region); }
			if (properties.formatting.weight.has_value()) { dw_layout->SetFontWeight    (utils::MS::raw::graphics::dw::cast(properties.formatting.weight.value())        , dx_region); }
			if (properties.formatting.style .has_value()) { dw_layout->SetFontStyle     (utils::MS::raw::graphics::dw::cast(properties.formatting.style .value())        , dx_region); }
			if (properties.formatting.locale.has_value()) { dw_layout->SetLocaleName    (utils::MS::string::utf8_to_wide   (properties.formatting.locale.value()).c_str(), dx_region); }

			const auto effects_com_ptr{utils::MS::raw::graphics::text::custom_renderer::effects::create(properties.rendering)};
			dw_layout->SetDrawingEffect(effects_com_ptr.get(), dx_region);
			}
		}

	void formatted_string::implementation::shrink_to_fit(formatted_string& formatted_string, float step)
		{
		DWRITE_TEXT_METRICS metrics;
		if (!dw_layout) { create_layout(formatted_string); }
		while (true)
			{
			dw_layout->GetMetrics(&metrics);
			if (metrics.height > metrics.layoutHeight)
				{
				float min_size{formatted_string.format.size};

				for (size_t i{0}; i < formatted_string.properties_regions.formatting.size.slots_size(); i++)
					{
					const auto slot{formatted_string.properties_regions.formatting.size.slot_at(i)};
					if (!slot.value_opt_ref.has_value()) { continue; }
					const auto& value{slot.value_opt_ref.value().get()};
					min_size = std::min(min_size, value);
					}

				if (min_size > step)
					{
					formatted_string.format.size -= step;

					for (size_t i{0}; i < formatted_string.properties_regions.formatting.size.slots_size(); i++)
						{
						auto slot{formatted_string.properties_regions.formatting.size.slot_at(i)};
						if (!slot.value_opt_ref.has_value()) { continue; }
						auto& value{slot.value_opt_ref.value().get()};
						value -= step;
						}

					create_layout(formatted_string);
					}
				else { break; }
				}
			else { break; }
			}
		}

	formatted_string::formatted_string(dx::initializer& dx_initializer) : implementation_ptr{utils::make_polymorphic_value<formatted_string::implementation>(dx_initializer)} {}
	formatted_string::formatted_string(dx::initializer& dx_initializer, const std::string& string, const text::format& format, const utils::math::vec2f& sizes) :
		string{string}, format{format}, sizes{sizes},
		implementation_ptr{utils::make_polymorphic_value<formatted_string::implementation>(dx_initializer)} {}

	formatted_string::~formatted_string() = default;

	void formatted_string::update() { implementation_ptr->create_layout(*this); }
	void formatted_string::shrink_to_fit(float step) { implementation_ptr->shrink_to_fit(*this, step); }
	}
