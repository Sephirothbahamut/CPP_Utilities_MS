#pragma once

#include <unordered_map>

#include <utils/input_system/device/keyboard.h>

#include "../window.h"

namespace utils::MS::window::input
	{
	class keyboard : public module
		{
		using input_keyboard = utils::input_system::device::keyboard;
		public:
			struct create_info 
				{
				utils::input_system::manager& input_manager;
				using module_type = keyboard;
				};

			keyboard(window::base& base, const create_info& create_info);

			std::unordered_map<uintptr_t, input_keyboard> keyboards;

			input_keyboard default_keyboard;

		private:
			utils::input_system::manager& input_manager;

			virtual procedure_result procedure(UINT msg, WPARAM wparam, LPARAM lparam) override;

			void default_process_digital(const input_keyboard::button_id& id, bool new_state) noexcept;

			bool wm_input(WPARAM wparam, LPARAM lparam);
		};

	}

#ifdef utils_implementation
#include "keyboard.cpp"
#endif