#pragma once

#include "common.h"

namespace utils::MS::raw::graphics::dw
	{
	struct factory : ms_wrapper<IDWriteFactory>
		{
		using ms_wrapper<IDWriteFactory>::ms_wrapper;
		factory(DWRITE_FACTORY_TYPE type = DWRITE_FACTORY_TYPE_SHARED);
		};
	}

#ifdef utils_implementation
#include "factory.cpp"
#endif