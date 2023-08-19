#pragma once

#include <utils/math/vec2.h>
#include <utils/math/rect.h>

#include <utils/MS/window/window.h>

class close_module : public utils::MS::window::module
	{
	public:
		struct create_info { using module_type = close_module; };

		close_module(utils::MS::window::base& base, const create_info& create_info = {}) : utils::MS::window::module{base} {}

	protected:
		virtual utils::MS::window::procedure_result procedure(UINT msg, WPARAM wparam, LPARAM lparam) override
			{
			if (msg == WM_KEYUP && wparam == VK_ESCAPE)
				{
				get_base().close();
				return utils::MS::window::procedure_result::stop(0);
				}
			else return utils::MS::window::procedure_result::next();
			}
	};