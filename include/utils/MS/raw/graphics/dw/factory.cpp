#include "factory.h"

namespace utils::MS::raw::graphics::dw::factory
	{
	com_ptr create(DWRITE_FACTORY_TYPE type)
		{
		com_ptr ret;
		winrt::check_hresult(DWriteCreateFactory(type, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(ret.put())));
		return ret;
		}
	}