#include "brush.h"

#include "cast.h"

namespace utils::MS::raw::graphics::d2d::brush
	{
	com_ptr create(winrt::com_ptr<ID2D1DeviceContext> d2d_context, const utils::graphics::colour::rgba_f& colour)
		{
		com_ptr ret;
		winrt::check_hresult(d2d_context->CreateSolidColorBrush(cast(colour), ret.put()));
		return ret;
		}
	}