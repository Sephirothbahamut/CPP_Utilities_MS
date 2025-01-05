#pragma once

#include <string>
#include <utils/math/vec.h>

#include "common.h"
#include "text_format.h"

namespace utils::MS::raw::graphics::dw::text_layout
	{
	using com_ptr = winrt::com_ptr<IDWriteTextLayout>;
	com_ptr create(winrt::com_ptr<IDWriteFactory> dw_factory, winrt::com_ptr<IDWriteTextFormat> text_format, const std::string& string, const utils::math::vec2f& sizes);
	}

#ifdef utils_implementation
#include "text_layout.cpp"
#endif