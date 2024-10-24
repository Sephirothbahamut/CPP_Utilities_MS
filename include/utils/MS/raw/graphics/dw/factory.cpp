#include "factory.h"

namespace utils::MS::raw::graphics::dw
	{
	factory::factory(DWRITE_FACTORY_TYPE type) : ms_wrapper{nullptr}
		{
		utils::MS::raw::throw_if_failed(DWriteCreateFactory(type, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&ptr)));
		}
	}