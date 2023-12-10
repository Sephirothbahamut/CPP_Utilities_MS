#include "passthrough.h"

#include "../window.h"

#include "../../windows.h"

namespace utils::MS::window::input
	{
	void passthrough::create_info::adjust_base_create_info(utils::MS::window::base::create_info& base_create_info)
		{
		base_create_info.style_ex |= WS_EX_TRANSPARENT | WS_EX_LAYERED;
		}

	passthrough::passthrough(utils::MS::window::base& base, const passthrough::create_info& create_info) :
		utils::MS::window::module{base}
		{
		//GetLayeredWindowAttributes succeeds if the style module was used to set transparency before; we do not override transparency set through it
		if (!GetLayeredWindowAttributes(get_base().get_handle(), nullptr, nullptr, nullptr))
			{
			SetLayeredWindowAttributes(get_base().get_handle(), 0, 255, LWA_ALPHA);
			}
		}
	}