#include "factory.h"

namespace utils::MS::raw::graphics::d2d::factory
	{
	com_ptr create(D2D1_FACTORY_TYPE type)
		{
		com_ptr ret;
		winrt::check_hresult(D2D1CreateFactory(type, ret.put()));
		return ret;
		}
	}