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

	formatted_string::implementation::implementation(dx::initializer& dx_initializer, const std::string& string, const format& format, const utils::math::vec2f& sizes) :
		dw_factory{dx_initializer.implementation_ptr->dw_factory},
		dw_format{utils::MS::raw::graphics::dw::text_format::create(dw_factory, format)},
		dw_layout{utils::MS::raw::graphics::dw::text_layout::create(dw_factory, dw_format, string, sizes)}
		{
		}

	void formatted_string::implementation::apply_properties(const utils::containers::regions<region::properties>& regions_properties)
		{
		for (size_t i{0}; i < regions_properties.slots_size(); i++)
			{
			const auto& slot{regions_properties.slot_at(i)};
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

	void formatted_string::implementation::shrink_to_fit()
		{
			
		}

	formatted_string::formatted_string(dx::initializer& dx_initializer) : implementation_ptr{utils::make_polymorphic_value<formatted_string::implementation>(dx_initializer)} {}
	formatted_string::formatted_string(dx::initializer& dx_initializer, const std::string& string, const format& format, const utils::math::vec2f& sizes) :
		implementation_ptr{utils::make_polymorphic_value<formatted_string::implementation>(dx_initializer, string, format, sizes)} {}

	formatted_string::~formatted_string() = default;

	void formatted_string::apply_properties(const utils::containers::regions<region::properties>& regions_properties) { implementation_ptr->apply_properties(regions_properties); }
	void formatted_string::shrink_to_fit() { implementation_ptr->shrink_to_fit(); }
	}
