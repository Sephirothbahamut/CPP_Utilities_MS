#pragma once

#include "common.h"

namespace utils::MS::raw::graphics::dw::factory
	{
	using com_ptr = winrt::com_ptr<IDWriteFactory>;
	com_ptr create(DWRITE_FACTORY_TYPE type = DWRITE_FACTORY_TYPE_SHARED);
	}

#ifdef utils_implementation
#include "factory.cpp"
#endif