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
		virtual utils::MS::window::procedure_result procedure(utils::MS::UINT msg, utils::MS::WPARAM wparam, utils::MS::LPARAM lparam) override;
	};