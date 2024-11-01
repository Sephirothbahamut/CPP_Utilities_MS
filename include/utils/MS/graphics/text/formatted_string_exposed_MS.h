#pragma once

#include "formatted_string.h"

#include "../../raw/graphics/text/custom_renderer.h"
#include "../../raw/graphics/dw/text_layout.h"
#include "../../raw/graphics/dw/text_format.h"
#include "../../raw/graphics/dw/factory.h"
#include "../../raw/graphics/dw/cast.h"
#include "../../string.h"

#include "../../raw/graphics/text/custom_renderer/effects.h"

namespace utils::MS::graphics::text
	{
	struct formatted_string::implementation
		{
		utils::MS::raw::graphics::dw::factory    ::com_ptr dw_factory {nullptr};
		utils::MS::raw::graphics::dw::text_layout::com_ptr text_layout{nullptr};
		utils::MS::raw::graphics::dw::text_format::com_ptr dw_format  {nullptr};
		utils::MS::raw::graphics::dw::text_layout::com_ptr dw_layout  {nullptr};

		implementation(dx::initializer& dx_initializer);
		implementation(dx::initializer& dx_initializer, const std::string& string, const format& format, const utils::math::vec2f& sizes);

		void apply_properties(const utils::containers::regions<region::properties>& regions_properties);

		void shrink_to_fit();
		};
	}

