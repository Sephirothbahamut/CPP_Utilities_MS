#pragma once

#include "common.h"
#include "../../../graphics/text/format.h"

namespace utils::MS::raw::graphics::dw
	{
	struct text_format : ms_wrapper<IDWriteTextFormat>
		{
		text_format(IDWriteFactory* dw_factory, const utils::MS::graphics::text::format& format);
		using ms_wrapper<IDWriteTextFormat>::ms_wrapper;
		};
	}

#ifdef utils_implementation
#include "text_format.cpp"
#endif