#include "factory.h"

namespace utils::MS::raw::graphics::d2d
	{
	factory::factory(D2D1_FACTORY_TYPE type)
		{
		winrt::check_hresult(D2D1CreateFactory(type, com_ptr.put()));
		}
	}