#pragma once

#include <array>
#include <cstdint>
#include <functional>
#include <unordered_map>

#include <utils/enum.h>
#include <utils/memory.h>
#include <utils/math/vec2.h>
#include <utils/math/vec3.h>
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

			mouse(window::base& base, const create_info& create_info) : module{base}, input_manager{create_info.input_manager}
				{
				//mouse
				RAWINPUTDEVICE rid_mouse
					{
					.usUsagePage{0x01},
					.usUsage    {0x02},//Mouse
				//	.usUsage    {0x05},//gamepad
				//	.usUsage    {0x06},//Keyboard
					.dwFlags    {RIDEV_INPUTSINK},
					.hwndTarget {base.get_handle()},
					};
				if (!RegisterRawInputDevices(&rid_mouse, 1, sizeof(rid_mouse))) { throw last_error("failed to register raw input devices"); }
				}

			std::unordered_map<uintptr_t, input_mouse> mice;

			input_mouse default_mouse;

		private:
			utils::input_system::manager& input_manager;

			void capture() const noexcept { SetCapture(get_base().get_handle()); }
			void release() const noexcept { ReleaseCapture(); }
			void prepare_mouseleave() const noexcept
				{
				TRACKMOUSEEVENT tme
					{
					.cbSize   {sizeof(TRACKMOUSEEVENT)},
					.dwFlags  {TME_LEAVE},
					.hwndTrack{get_base().get_handle()}
					};
				TrackMouseEvent(&tme);
				}
			
			utils::math::vec2l eval_vec2(LPARAM lparam) const noexcept
				{
				return {GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam)};
				}
			utils::input_system::device::mouse::button_id eval_extra_button(WPARAM wparam) const noexcept
				{
				return ((GET_XBUTTON_WPARAM(wparam) == XBUTTON1) ? utils::input_system::device::mouse::button_id::backward : utils::input_system::device::mouse::button_id::forward);
				}

			virtual procedure_result procedure(UINT msg, WPARAM wparam, LPARAM lparam) override
				{
				switch (msg)
					{
					case WM_MOUSEMOVE  : prepare_mouseleave(); default_process_analog (eval_vec2(lparam)                    ); return procedure_result::next(0);
					case WM_LBUTTONDOWN: capture           (); default_process_digital(input_mouse::button_id::left  , true ); return procedure_result::next(0);
					case WM_LBUTTONUP  : release           (); default_process_digital(input_mouse::button_id::left  , false); return procedure_result::next(0);
					case WM_RBUTTONDOWN: capture           (); default_process_digital(input_mouse::button_id::right , true ); return procedure_result::next(0);
					case WM_RBUTTONUP  : release           (); default_process_digital(input_mouse::button_id::right , false); return procedure_result::next(0);
					case WM_MBUTTONDOWN: capture           (); default_process_digital(input_mouse::button_id::middle, true ); return procedure_result::next(0);
					case WM_MBUTTONUP  : release           (); default_process_digital(input_mouse::button_id::middle, false); return procedure_result::next(0);
					case WM_XBUTTONDOWN: capture           (); default_process_digital(eval_extra_button(wparam)     , true ); return procedure_result::next(0);
					case WM_XBUTTONUP  : release           (); default_process_digital(eval_extra_button(wparam)     , false); return procedure_result::next(0);
					//case WM_MOUSELEAVE :                       default_mouse.leave                                          .trigger     (                 ); return procedure_result::next(0);
					case WM_INPUT      : if(wm_input(wparam, lparam)) { return procedure_result::next(0); }
					}
				
				return procedure_result::next();
				}

			void default_process_digital(const input_mouse::button_id& id, bool new_state) noexcept
				{
				default_mouse.digital.change(input_manager, id, new_state);
				}
			void default_process_analog(utils::math::vec2f new_state) noexcept
				{
				default_mouse.analog.change(input_manager, input_mouse::axis_id::x, new_state.x);
				default_mouse.analog.change(input_manager, input_mouse::axis_id::y, new_state.y);
				}

			bool wm_input(WPARAM wparam, LPARAM lparam)
				{
				UINT dataSize;
				GetRawInputData(reinterpret_cast<HRAWINPUT>(lparam), RID_INPUT, NULL, &dataSize, sizeof(RAWINPUTHEADER)); //Need to populate data size first
					
				if (dataSize == 0) { return false; }

				std::vector<BYTE> rawdata(dataSize);

				if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lparam), RID_INPUT, rawdata.data(), &dataSize, sizeof(RAWINPUTHEADER)) == dataSize)
					{
					utils::observer_ptr<RAWINPUT> raw{reinterpret_cast<utils::observer_ptr<RAWINPUT>>(rawdata.data())};

					if (raw->header.dwType != RIM_TYPEMOUSE) { return false; }

					auto device_handle{reinterpret_cast<uintptr_t>(raw->header.hDevice)};

					auto result{mice.emplace(std::piecewise_construct, std::make_tuple(device_handle), std::make_tuple())};
					auto& mouse{result.first->second};

					// https://docs.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-rawmouse
					//TODO wheel
					const auto& rawmouse{raw->data.mouse};

					const bool global{GET_RAWINPUT_CODE_WPARAM(wparam) == RIM_INPUTSINK};

					if ((rawmouse.usFlags & MOUSE_MOVE_ABSOLUTE) == MOUSE_MOVE_ABSOLUTE)
						{
						const bool isVirtualDesktop = (rawmouse.usFlags & MOUSE_VIRTUAL_DESKTOP) == MOUSE_VIRTUAL_DESKTOP;

						const int width  = GetSystemMetrics(isVirtualDesktop ? SM_CXVIRTUALSCREEN : SM_CXSCREEN);
						const int height = GetSystemMetrics(isVirtualDesktop ? SM_CYVIRTUALSCREEN : SM_CYSCREEN);

						auto const x = static_cast<long>((float(rawmouse.lLastX) / 65535.0f) * float(width ));
						auto const y = static_cast<long>((float(rawmouse.lLastY) / 65535.0f) * float(height));

						//Move to
						mouse.analog[input_mouse::axis_id::x].change(input_manager, x);
						mouse.analog[input_mouse::axis_id::y].change(input_manager, y);
						}
					else if (rawmouse.lLastX != 0 || rawmouse.lLastY != 0)
						{
						long x = rawmouse.lLastX;
						long y = rawmouse.lLastY;

						//Move by
						mouse.analog[input_mouse::axis_id::x].change(input_manager, x);
						mouse.analog[input_mouse::axis_id::y].change(input_manager, y);
						}

					if (rawmouse.usButtonFlags & RI_MOUSE_BUTTON_1_DOWN) { mouse.digital[input_mouse::button_id::left    ].change(input_manager, true ); }
					if (rawmouse.usButtonFlags & RI_MOUSE_BUTTON_1_UP  ) { mouse.digital[input_mouse::button_id::left    ].change(input_manager, false); }
					if (rawmouse.usButtonFlags & RI_MOUSE_BUTTON_2_DOWN) { mouse.digital[input_mouse::button_id::right   ].change(input_manager, true ); }
					if (rawmouse.usButtonFlags & RI_MOUSE_BUTTON_2_UP  ) { mouse.digital[input_mouse::button_id::right   ].change(input_manager, false); }
					if (rawmouse.usButtonFlags & RI_MOUSE_BUTTON_3_DOWN) { mouse.digital[input_mouse::button_id::middle  ].change(input_manager, true ); }
					if (rawmouse.usButtonFlags & RI_MOUSE_BUTTON_3_UP  ) { mouse.digital[input_mouse::button_id::middle  ].change(input_manager, false); }
					if (rawmouse.usButtonFlags & RI_MOUSE_BUTTON_4_DOWN) { mouse.digital[input_mouse::button_id::backward].change(input_manager, true ); }
					if (rawmouse.usButtonFlags & RI_MOUSE_BUTTON_4_UP  ) { mouse.digital[input_mouse::button_id::backward].change(input_manager, false); }
					if (rawmouse.usButtonFlags & RI_MOUSE_BUTTON_5_DOWN) { mouse.digital[input_mouse::button_id::forward ].change(input_manager, true ); }
					if (rawmouse.usButtonFlags & RI_MOUSE_BUTTON_5_UP  ) { mouse.digital[input_mouse::button_id::forward ].change(input_manager, false); }
					}

				return true;
				}
		};

	}