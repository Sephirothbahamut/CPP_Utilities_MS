#include "render_target.h"

namespace utils::MS::raw::graphics::d2d::hwnd_render_target
	{
	com_ptr create(d2d::factory::com_ptr& d2d_factory, const create_info& create_info)
		{
		com_ptr ret{nullptr};
		winrt::check_hresult(d2d_factory->CreateHwndRenderTarget(create_info.properties, D2D1::HwndRenderTargetProperties(create_info.hwnd), ret.put()));
		return ret;
		}
	}