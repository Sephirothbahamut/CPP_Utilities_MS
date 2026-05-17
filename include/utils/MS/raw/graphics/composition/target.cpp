#include "target.h"


namespace utils::MS::raw::graphics::composition
	{
	target::target(IDCompositionDevice* composition_device, HWND hwnd)
		{
		winrt::check_hresult(composition_device->CreateTargetForHwnd(hwnd, TRUE, com_ptr.put()));
		}
	}