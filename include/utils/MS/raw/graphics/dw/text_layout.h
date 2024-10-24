#pragma once

#include <string>
#include <utils/math/vec2.h>

#include "common.h"
#include "text_format.h"

namespace utils::MS::raw::graphics::dw
	{
	struct text_layout : ms_wrapper<IDWriteTextLayout>
		{
		using ms_wrapper<IDWriteTextLayout>::ms_wrapper;
		text_layout(IDWriteFactory* dw_factory, IDWriteTextFormat* text_format, const std::string& string, const utils::math::vec2f& sizes);
		};
	}

#ifdef utils_implementation
#include "text_layout.cpp"
#endif