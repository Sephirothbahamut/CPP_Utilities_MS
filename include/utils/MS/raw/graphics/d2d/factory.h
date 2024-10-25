#pragma once

#include "common.h"

namespace utils::MS::raw::graphics::d2d::factory
	{
	using com_ptr = winrt::com_ptr<ID2D1Factory1>;
	com_ptr create(D2D1_FACTORY_TYPE type = D2D1_FACTORY_TYPE_MULTI_THREADED);
	}

#ifdef utils_implementation
#include "factory.cpp"
#endif