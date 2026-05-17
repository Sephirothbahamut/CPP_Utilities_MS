#include "visual.h"

namespace utils::MS::raw::graphics::composition
	{
	visual::visual(IDCompositionDevice* composition_device)
		{
		winrt::check_hresult(composition_device->CreateVisual(com_ptr.put()));
		}
	}