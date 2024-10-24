#pragma once

#include "common.h"

namespace utils::MS::raw::graphics::d2d
	{
	struct factory : ms_wrapper<ID2D1Factory1>
		{
		using ms_wrapper<ID2D1Factory1>::ms_wrapper;
		factory(D2D1_FACTORY_TYPE type = D2D1_FACTORY_TYPE_MULTI_THREADED);
		};
	}

#ifdef utils_implementation
#include "factory.cpp"
#endif