#include "formatted_string_exposed_MS.h"

#include <utils/string.h>

#include "../dx/initializer/exposed_MS.h"

namespace utils::MS::graphics::text
	{
	DWRITE_TEXT_RANGE cast(const utils::containers::region& region)
		{
		const auto uint32_begin{utils::math::cast_clamp<UINT32>(region.begin)};
		const auto uint32_end  {utils::math::cast_clamp<UINT32>(region.count)};
		if (utils::math::will_overflow_sum(uint32_begin, uint32_end))
			{
			const auto adapted_end{std::numeric_limits<UINT32>::max() - uint32_begin};
			return {uint32_begin, adapted_end};
			}
		else
			{
			return {uint32_begin, uint32_end};
			}
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

	utils::math::rect<float> formatted_string::renderable::glyphs_enclosing_rect() const noexcept
		{
		const auto& layout{implementation_ptr->dw_layout};
		DWRITE_TEXT_METRICS metrics{};
		if (S_OK != layout->GetMetrics(std::addressof(metrics)))
			{
			return utils::math::rect<float>::create::infinite();
			}
		return utils::math::rect<float>::create::from_possize(utils::math::vec2f{metrics.left, metrics.top}, utils::math::vec2f{metrics.width, metrics.height});
		}

	utils::math::rect<float> formatted_string::renderable::glyphs_enclosing_rect_with_overhangs() const noexcept
		{
		const auto& layout{implementation_ptr->dw_layout};
		DWRITE_OVERHANG_METRICS metrics{};
		if (S_OK != layout->GetOverhangMetrics(std::addressof(metrics)))
			{
			return utils::math::rect<float>::create::infinite();
			}
		return utils::math::rect<float>{metrics.left, metrics.top, metrics.right, metrics.bottom};
		}


	void for_each_slot(const auto& regions, auto callback)
		{
		for (const auto& slot : regions.slot_index_view())
			{
			const auto dx_region{cast(slot.region)};
			callback(slot.value, dx_region);
			}
		}


	void formatted_string::renderable::implementation::create_layout(const formatted_string& formatted_string)
		{
		auto dw_format{utils::MS::raw::graphics::dw::text_format::create(dw_factory, formatted_string.format)};
		dw_layout = utils::MS::raw::graphics::dw::text_layout::create(dw_factory, dw_format, formatted_string.string, formatted_string.sizes);
		
		for_each_slot(formatted_string.properties_regions.format.font, [&](const std::u16string& value, DWRITE_TEXT_RANGE region)
			{
			if (value != formatted_string.format.font)
				{
				dw_layout->SetFontFamilyName(utils::string::cast<wchar_t>(value).c_str(), region);
				}
			});
		for_each_slot(formatted_string.properties_regions.format.locale, [&](const std::u16string& value, DWRITE_TEXT_RANGE region)
			{
			if (value != formatted_string.format.locale)
				{
				dw_layout->SetFontFamilyName(utils::string::cast<wchar_t>(value).c_str(), region);
				}
			});

		for_each_slot(formatted_string.properties_regions.format.size                 , [&](const float                     & value, DWRITE_TEXT_RANGE region) { if(value != formatted_string.format.size  ) { dw_layout->SetFontSize  (                                   value , region); } });
		for_each_slot(formatted_string.properties_regions.format.weight               , [&](const MS::graphics::text::weight& value, DWRITE_TEXT_RANGE region) { if(value != formatted_string.format.weight) { dw_layout->SetFontWeight(utils::MS::raw::graphics::dw::cast(value), region); } });
		for_each_slot(formatted_string.properties_regions.format.style                , [&](const MS::graphics::text::style & value, DWRITE_TEXT_RANGE region) { if(value != formatted_string.format.style ) { dw_layout->SetFontStyle (utils::MS::raw::graphics::dw::cast(value), region); } });
		for_each_slot(formatted_string.properties_regions.format.strikethrough.enabled, [&](const bool                      & value, DWRITE_TEXT_RANGE region) { dw_layout->SetStrikethrough(value , region); });
		for_each_slot(formatted_string.properties_regions.format.underline    .enabled, [&](const bool                      & value, DWRITE_TEXT_RANGE region) { dw_layout->SetUnderline    (value , region); });


		using aggregate_properties_t = utils::containers::aggregate_regions
			<
			utils::MS::graphics::text::regions::properties,
			utils::MS::graphics::text::regions::properties::regions,
			utils::MS::graphics::text::regions::properties::accessors_helper,
			true
			>;
		const aggregate_properties_t aggregate_properties{formatted_string.properties_regions};

		const auto split_indices{[&]()
			{
			auto tmp{aggregate_properties.split_indices_set()};
			std::copy(formatted_string.custom_splits.begin(), formatted_string.custom_splits.end(), std::inserter(tmp, tmp.end()));
			tmp.insert(0);
			tmp.insert(std::numeric_limits<size_t>::max());
			return std::vector<size_t>{tmp.begin(), tmp.end()};
			}()};
		if (!split_indices.empty())
			{
			assert(split_indices.size() > 1);
			for (size_t i = 0; i < (split_indices.size() - 1); i++)
				{
				const utils::containers::region split_region{utils::containers::region::create::from_to(split_indices[i], split_indices[i + 1])};
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





		//// Declare a typography pointer.
		//IDWriteTypography* typography_ptr = NULL;
		//
		//// Create a typography interface object.
		//dw_factory->CreateTypography(&typography_ptr);
		//
		//DWRITE_FONT_FEATURE font_feature{.nameTag{DWRITE_FONT_FEATURE_TAG_STYLISTIC_SET_7}, .parameter{1}};
		//typography_ptr->AddFontFeature(font_feature);
		//
		//dw_layout->SetTypography(typography_ptr, DWRITE_TEXT_RANGE{0, 28});
		dw_layout->SetLineSpacing(DWRITE_LINE_SPACING_METHOD_PROPORTIONAL, 0.6f, 1.f);


		while (true)
			{
			dw_layout->GetMetrics(&metrics);
			
			if (metrics.height > metrics.layoutHeight)
				{
				float min_size{formatted_string.format.size};

				for (const auto& slot : formatted_string.properties_regions.format.size)
					{
					const float& value{slot.value};
					min_size = std::min(min_size, value);
					}

				if (min_size > step)
					{
					formatted_string.format.size -= step;

					for (const auto& slot : formatted_string.properties_regions.format.size.unsafe_slots_index_view())
						{
						if (slot.value)
							{
							slot.value -= step;
							}
						}

					create_layout(formatted_string);
					}
				else { break; }
				}
			else { break; }
			}
		}

	formatted_string::formatted_string(const formatted_string::create_info& create_info) : formatted_string{create_info.string, create_info.format, create_info.sizes} {}
	formatted_string::formatted_string(const std::u16string& string, const text::format& format, const utils::math::vec2f& sizes) :
		string{string},
		format{format},
		sizes {sizes },
		properties_regions{utils::MS::graphics::text::regions::properties::regions::create::from_base_format(format)}
		{
		}


	formatted_string::formatted_string(const std::u16string& string, const text::format& format, const utils::math::vec2f& sizes, const utils::MS::graphics::text::regions::properties::regions& properties_regions) :
		string{string},
		format{format},
		sizes{sizes},
		properties_regions{properties_regions}
		{}

	void formatted_string::reset_properties_regions_to_format() noexcept
		{
		properties_regions = utils::MS::graphics::text::regions::properties::regions::create::from_base_format(format);
		}

	void formatted_string::chage_font_sizes(float delta, float minimum, float maximum) noexcept
		{
		if (delta > 0.f) 
			{
			increase_font_sizes( delta, maximum); 
			}
		else
			{
			decrease_font_sizes(-delta, minimum);
			}
		}

	bool formatted_string::increase_font_sizes(float delta, float maximum) noexcept
		{
		assert(delta >= 0.f);
		float current_max{format.size};
		for (const auto& slot : properties_regions.format.size.slot_index_view())
			{
			current_max = std::max(current_max, slot.value);
			}
		if (current_max > maximum)
			{
			return false; 
			}

		const float max_delta{maximum - current_max};
		delta = std::min(delta, max_delta);

		format.size += delta;
		for (auto slot : properties_regions.format.size.unsafe_slots_index_view())
			{
			slot.value += delta;
			}
		return true;
		}

	bool formatted_string::decrease_font_sizes(float delta, float minimum) noexcept
		{
		delta *= -1.f;
		assert(delta <= 0.f);

		float current_min{format.size};
		for (const auto& slot : properties_regions.format.size.slot_index_view())
			{
			current_min = std::min(current_min, slot.value);
			}
		if (current_min <= minimum)
			{
			return false;
			}

		const float min_delta{minimum - current_min};
		//if (min_delta <= 0.f) {}
		delta = std::max(delta, min_delta);

		format.size += delta;
		for (auto slot : properties_regions.format.size.unsafe_slots_index_view())
			{
			slot.value += delta;
			}
		return true;
		}

	utils::math::rect<float> formatted_string::glyphs_enclosing_rect(dx::initializer& dx_initializer) const noexcept
		{
		const auto renderable{finalize(dx_initializer)};
		return renderable.glyphs_enclosing_rect();
		}

	utils::math::rect<float> formatted_string::glyphs_enclosing_rect_with_overhangs(dx::initializer& dx_initializer) const noexcept
		{
		const auto renderable{finalize(dx_initializer)};
		return renderable.glyphs_enclosing_rect_with_overhangs();
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
