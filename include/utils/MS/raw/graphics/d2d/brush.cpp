#include "brush.h"

#include "cast.h"

namespace utils::MS::raw::graphics::d2d
	{
	brush::brush(ID2D1DeviceContext* d2d_context, const utils::graphics::colour::rgba_f& colour) : ms_wrapper{nullptr}
		{
		throw_if_failed(d2d_context->CreateSolidColorBrush(cast(colour), &ptr));
		}
	}