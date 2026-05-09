#include "target.h"


namespace utils::MS::raw::graphics::d2d::composition
	{
	target::target(const composition::device& composition_device, HWND hwnd)
		{
		winrt::check_hresult(composition_device->CreateTargetForHwnd(hwnd, TRUE, com_ptr.put()));
		}
	}