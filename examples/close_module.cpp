#include "close_module.h"

#include <utils/MS/windows.h>

utils::MS::window::procedure_result close_module::procedure(utils::MS::UINT msg, utils::MS::WPARAM wparam, utils::MS::LPARAM lparam)
	{
	if (msg == WM_KEYUP && wparam == VK_ESCAPE)
		{
		get_base().close();
		return utils::MS::window::procedure_result::stop(0);
		}
	else return utils::MS::window::procedure_result::next();
	}