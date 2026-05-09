#include "visual.h"

namespace utils::MS::raw::graphics::d2d::composition
	{
	visual::visual(const composition::device& composition_device)
		{
		winrt::check_hresult(composition_device->CreateVisual(com_ptr.put()));
		}
	}