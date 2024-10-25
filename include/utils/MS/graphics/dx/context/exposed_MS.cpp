#include "exposed_MS.h"

namespace utils::MS::graphics::dx
	{
	context::implementation::implementation(utils::MS::raw::graphics::d2d::device::com_ptr d2d_device) : 
		d2d_context{utils::MS::raw::graphics::d2d::context::create(d2d_device)} 
		{}
	}