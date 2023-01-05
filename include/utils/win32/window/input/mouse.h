#pragma once

#include <array>
#include <cstdint>
#include <functional>
#include <unordered_map>

#include <utils/enum.h>
#include <utils/memory.h>
#include <utils/math/vec2.h>
#include <utils/math/vec3.h>

#include "../window.h"
#include "../../../input/mouse.h"


// For future textbox shortcuts
// https://learn.microsoft.com/en-us/windows/win32/learnwin32/accelerator-tables

namespace utils::win32::window::input
	{
	class mouse : public module
		{
		public:
			mouse(window::base& base) : 
				module
					{
					base,
					[this](UINT msg, WPARAM wparam, LPARAM lparam) -> std::optional<LRESULT> { return procedure(msg, wparam, lparam); }
					}
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

			std::unordered_map<uintptr_t, std::reference_wrapper<utils::input::mouse>> mice;

			utils::input::mouse default_mouse;

		private:

			utils::math::vec2l eval_vec2(LPARAM lparam) const noexcept
				{
				return {GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam)};
				}
			utils::input::mouse::button_id eval_extra_button(WPARAM wparam) const noexcept
				{
				return ((GET_XBUTTON_WPARAM(wparam) == XBUTTON1) ? utils::input::mouse::button_id::backward : utils::input::mouse::button_id::forward);
				}

			std::optional<LRESULT> procedure(UINT msg, WPARAM wparam, LPARAM lparam)
				{
				switch (msg)
					{
					case WM_MOUSEMOVE  : default_mouse.position                          .change_state(eval_vec2(lparam)); return 0;
					case WM_LBUTTONDOWN: default_mouse.buttons.left                      .change_state(true             ); return 0;
					case WM_LBUTTONUP  : default_mouse.buttons.left                      .change_state(false            ); return 0;
					case WM_RBUTTONDOWN: default_mouse.buttons.right                     .change_state(true             ); return 0;
					case WM_RBUTTONUP  : default_mouse.buttons.right                     .change_state(false            ); return 0;
					case WM_MBUTTONDOWN: default_mouse.buttons.middle                    .change_state(true             ); return 0;
					case WM_MBUTTONUP  : default_mouse.buttons.middle                    .change_state(false            ); return 0;
					case WM_XBUTTONDOWN: default_mouse.buttons[eval_extra_button(wparam)].change_state(true             ); return 0;
					case WM_XBUTTONUP  : default_mouse.buttons[eval_extra_button(wparam)].change_state(false            ); return 0;
					case WM_INPUT      : if(wm_input(wparam, lparam)) { return 0; }
					}
				return std::nullopt;
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

					auto mouse_ref_it{mice.find(device_handle)};
					if (mouse_ref_it == mice.end()) { return false; }
					auto& mouse{mouse_ref_it->second.get()};

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

						mouse.position.change_state({x, y});
						}
					else if (rawmouse.lLastX != 0 || rawmouse.lLastY != 0)
						{
						long x = rawmouse.lLastX;
						long y = rawmouse.lLastY;

						mouse.move_by({x, y});
						}

					if (rawmouse.usButtonFlags & RI_MOUSE_BUTTON_1_DOWN) { mouse.buttons.left    .change_state(true ); }
					if (rawmouse.usButtonFlags & RI_MOUSE_BUTTON_1_UP  ) { mouse.buttons.left    .change_state(false); }
					if (rawmouse.usButtonFlags & RI_MOUSE_BUTTON_2_DOWN) { mouse.buttons.right   .change_state(true ); }
					if (rawmouse.usButtonFlags & RI_MOUSE_BUTTON_2_UP  ) { mouse.buttons.right   .change_state(false); }
					if (rawmouse.usButtonFlags & RI_MOUSE_BUTTON_3_DOWN) { mouse.buttons.middle  .change_state(true ); }
					if (rawmouse.usButtonFlags & RI_MOUSE_BUTTON_3_UP  ) { mouse.buttons.middle  .change_state(false); }
					if (rawmouse.usButtonFlags & RI_MOUSE_BUTTON_4_DOWN) { mouse.buttons.backward.change_state(true ); }
					if (rawmouse.usButtonFlags & RI_MOUSE_BUTTON_4_UP  ) { mouse.buttons.backward.change_state(false); }
					if (rawmouse.usButtonFlags & RI_MOUSE_BUTTON_5_DOWN) { mouse.buttons.forward .change_state(true ); }
					if (rawmouse.usButtonFlags & RI_MOUSE_BUTTON_5_UP  ) { mouse.buttons.forward .change_state(false); }
					}

				return true;
				}
		};

	}