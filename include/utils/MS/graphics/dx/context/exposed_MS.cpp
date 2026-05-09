#include "exposed_MS.h"

namespace utils::MS::graphics::dx
	{
	context::implementation::implementation(utils::MS::raw::graphics::d2d::device& d2d_device) : 
		d2d_context{d2d_device} 
		{}
	}