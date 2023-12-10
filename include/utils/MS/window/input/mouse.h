#pragma once

#include <unordered_map>

#include <utils/input_system/device/mouse.h>

#include "../window.h"

// For future textbox shortcuts
// https://learn.microsoft.com/en-us/windows/win32/learnwin32/accelerator-tables

namespace utils::MS::window::input
	{
	class mouse : public module
		{
		using input_mouse = utils::input_system::device::mouse;
		public:
			struct create_info 
				{
				utils::input_system::manager& input_manager;
				using module_type = mouse; 
				};

			mouse(window::base& base, const create_info& create_info);

			std::unordered_map<uintptr_t, input_mouse> mice;

			input_mouse default_mouse;

		private:
			utils::input_system::manager& input_manager;

			void capture() const noexcept;
			void release() const noexcept;
			void prepare_mouseleave() const noexcept;
			
			utils::math::vec2l eval_vec2(LPARAM lparam) const noexcept;
			utils::input_system::device::mouse::button_id eval_extra_button(WPARAM wparam) const noexcept;

			virtual procedure_result procedure(UINT msg, WPARAM wparam, LPARAM lparam) override;

			void default_process_digital(const input_mouse::button_id& id, bool new_state) noexcept;
			void default_process_analog(utils::math::vec2f new_state) noexcept;


			bool wm_input(WPARAM wparam, LPARAM lparam);
		};
	}

#ifdef utils_implementation
#include "mouse.cpp"
#endif