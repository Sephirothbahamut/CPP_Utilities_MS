#pragma once

#include "common.h"

namespace utils::MS::raw::graphics::d2d
	{
	struct factory : com_wrapper<ID2D1Factory1>
		{
		factory(D2D1_FACTORY_TYPE type = D2D1_FACTORY_TYPE_MULTI_THREADED);
		};
	}

#ifdef utils_implementation
#include "factory.cpp"
#endif