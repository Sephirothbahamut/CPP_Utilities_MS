#include "formatted_string.h"

#include "../../raw/graphics/dw/text_layout.h"
#include "../../raw/graphics/dw/text_format.h"
#include "../../raw/graphics/dw/factory.h"
#include "../../raw/graphics/text/custom_renderer.h"

//#include "effect/implementation_base_exposed_MS.h"

namespace utils::MS::graphics::text
	{
	struct formatted_string::implementation : utils::oop::non_copyable, utils::oop::non_movable
		{
		utils::MS::raw::graphics::dw::factory    ::com_ptr dw_factory {nullptr};
		utils::MS::raw::graphics::dw::text_layout::com_ptr text_layout{nullptr};
		utils::MS::raw::graphics::dw::text_format::com_ptr dw_format  {nullptr};
		utils::MS::raw::graphics::dw::text_layout::com_ptr dw_layout  {nullptr};

		implementation() = default;
		implementation(const std::string& string, const format& format, const utils::math::vec2f& sizes) :
			dw_factory{utils::MS::raw::graphics::dw::factory::create()},
			dw_format{utils::MS::raw::graphics::dw::text_format::create(dw_factory, format)},
			dw_layout{utils::MS::raw::graphics::dw::text_layout::create(dw_factory, dw_format, string, sizes)}
			{
			}

		//void add_effect(effect::base& effect, region region)
		//	{
		//	auto& com_effect_wrapper{effect.implementation_ptr->com_effect_ptr};
		//	dw_layout->SetDrawingEffect(com_effect_wrapper.get(), {.startPosition{static_cast<uint32_t>(region.begin)}, .length{static_cast<uint32_t>(region.count)}});
		//	}
		};


	formatted_string::formatted_string() : implementation_ptr{std::make_unique<formatted_string::implementation>()} {}
	formatted_string::formatted_string(const std::string& string, const format& format, const utils::math::vec2f& sizes) :
		implementation_ptr{std::make_unique<formatted_string::implementation>(string, format, sizes)} {}

	formatted_string::~formatted_string() = default;

	//void formatted_string::add_effect(effect::base& effect, region region) { implementation_ptr->add_effect(effect, region); }
	}
