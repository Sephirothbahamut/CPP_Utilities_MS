#pragma once

#include "../window.h"


namespace utils::MS::window::input
	{
	/// <summary> Enables input passthrough. Any input will not affect this window and pass to whatever is behind it. </summary>
	struct passthrough : utils::MS::window::module
		{
		struct create_info
			{
			using module_type = passthrough;
			void adjust_base_create_info(utils::MS::window::base::create_info& base_create_info)
				{
				base_create_info.style_ex |= WS_EX_TRANSPARENT | WS_EX_LAYERED;
				}
			};

		passthrough(utils::MS::window::base& base, const create_info& create_info = {}) :
			utils::MS::window::module{base}
			{
			//GetLayeredWindowAttributes succeeds if the style module was used to set transparency before; we do not override transparency set through it
			if (!GetLayeredWindowAttributes(get_base().get_handle(), nullptr, nullptr, nullptr))
				{
				SetLayeredWindowAttributes(get_base().get_handle(), 0, 255, LWA_ALPHA);
				}
			}
		};
	}