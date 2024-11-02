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
		utils::MS::raw::graphics::dw::factory    ::com_ptr dw_factory{nullptr};
		utils::MS::raw::graphics::dw::text_layout::com_ptr dw_layout {nullptr};

		implementation(dx::initializer& dx_initializer);

		void create_layout(const formatted_string& formatted_string);
		void shrink_to_fit(formatted_string& formatted_string, float step);
		};
	}

