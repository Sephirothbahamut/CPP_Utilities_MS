#pragma once

#include "common.h"
#include "../../../graphics/text/format.h"

namespace utils::MS::raw::graphics::dw::text_format
	{
	using com_ptr = winrt::com_ptr<IDWriteTextFormat>;
	com_ptr create(winrt::com_ptr<IDWriteFactory> dw_factory, const utils::MS::graphics::text::format& format);
	}

#ifdef utils_implementation
#include "text_format.cpp"
#endif