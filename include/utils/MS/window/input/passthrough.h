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
			void adjust_base_create_info(utils::MS::window::base::create_info& base_create_info);
			};

		passthrough(utils::MS::window::base& base, const create_info& create_info = {});
		};
	}

#ifdef utils_implementation
#include "passthrough.cpp"
#endif