#pragma once

#include "common.h"

#include "../../../graphics/text/format.h"

namespace utils::MS::raw::graphics::dw
	{
	DWRITE_TEXT_ALIGNMENT      cast(utils::alignment::horizontal      alignment) noexcept;
	DWRITE_PARAGRAPH_ALIGNMENT cast(utils::alignment::vertical        alignment) noexcept;
	DWRITE_FONT_WEIGHT         cast(utils::MS::graphics::text::weight weight   ) noexcept;
	DWRITE_FONT_STYLE          cast(utils::MS::graphics::text::style  style    ) noexcept;
	}

#ifdef utils_implementation
#include "cast.cpp"
#endif