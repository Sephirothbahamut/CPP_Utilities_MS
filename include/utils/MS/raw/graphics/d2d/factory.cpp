#include "factory.h"

namespace utils::MS::raw::graphics::d2d
	{
	factory::factory(D2D1_FACTORY_TYPE type) : ms_wrapper{nullptr}
		{
		utils::MS::raw::throw_if_failed(D2D1CreateFactory(type, &ptr));
		}
	}