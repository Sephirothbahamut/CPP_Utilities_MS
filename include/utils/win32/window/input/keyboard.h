//class input : public virtual utils::win32::window::base
//	{
//	public:
//		struct create_info { utils::input::manager& input_manager; };
//
//		input(create_info create_info) : input_manager_ptr{&create_info.input_manager} {}
//
//		std::optional<LRESULT> procedure(UINT msg, WPARAM wparam, LPARAM lparam)
//			{
//			switch (msg)
//				{
//				case WM_ACTIVATE:
//				case WM_ACTIVATEAPP:
//				case WM_MOUSEMOVE:
//				case WM_LBUTTONDOWN:
//				case WM_LBUTTONUP:
//				case WM_RBUTTONDOWN:
//				case WM_RBUTTONUP:
//				case WM_MBUTTONDOWN:
//				case WM_MBUTTONUP:
//				case WM_MOUSEWHEEL:
//				case WM_XBUTTONDOWN:
//				case WM_XBUTTONUP:
//				case WM_MOUSEHOVER:
//					break;
//
//				case WM_INPUT:
//				{
//				UINT dataSize;
//				GetRawInputData(reinterpret_cast<HRAWINPUT>(lparam), RID_INPUT, NULL, &dataSize, sizeof(RAWINPUTHEADER)); //Need to populate data size first
//				//std::cout << GET_RAWINPUT_CODE_WPARAM(wparam) << " code thing\n";
//				if (dataSize > 0)
//					{
//					std::vector<BYTE> rawdata(dataSize);
//
//					if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lparam), RID_INPUT, rawdata.data(), &dataSize, sizeof(RAWINPUTHEADER)) == dataSize)
//						{
//						RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(rawdata.data());
//						switch (raw->header.dwType)
//							{
//							case RIM_TYPEMOUSE:
//							{
//							// https://docs.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-rawmouse
//							//TODO wheel
//							const auto& rawmouse{raw->data.mouse};
//
//							if ((rawmouse.usFlags & MOUSE_MOVE_ABSOLUTE) == MOUSE_MOVE_ABSOLUTE)
//								{
//								const bool isVirtualDesktop = (rawmouse.usFlags & MOUSE_VIRTUAL_DESKTOP) == MOUSE_VIRTUAL_DESKTOP;
//
//								const int width  = GetSystemMetrics(isVirtualDesktop ? SM_CXVIRTUALSCREEN : SM_CXSCREEN);
//								const int height = GetSystemMetrics(isVirtualDesktop ? SM_CYVIRTUALSCREEN : SM_CYSCREEN);
//
//								auto const x = static_cast<long>((float(rawmouse.lLastX) / 65535.0f) * float(width ));
//								auto const y = static_cast<long>((float(rawmouse.lLastY) / 65535.0f) * float(height));
//
//								//TODO lLastX/lLastY or x/y?
//								input_manager_ptr->mouse.move_to({.device_handle{reinterpret_cast<uintptr_t>(raw->header.hDevice)}, .position{x, y}});
//								}
//							else if (rawmouse.lLastX != 0 || rawmouse.lLastY != 0)
//								{
//								int relativeX = rawmouse.lLastX;
//								int relativeY = rawmouse.lLastY;
//								
//								input_manager_ptr->mouse.move_by({.device_handle{reinterpret_cast<uintptr_t>(raw->header.hDevice)}, .delta{relativeX, relativeY}});
//								}
//							if (rawmouse.usButtonFlags & RI_MOUSE_BUTTON_1_DOWN) { input_manager_ptr->mouse.button_pressed ({.device_handle{reinterpret_cast<uintptr_t>(raw->header.hDevice)}, .button_index{1}}); }
//							if (rawmouse.usButtonFlags & RI_MOUSE_BUTTON_1_UP  ) { input_manager_ptr->mouse.button_released({.device_handle{reinterpret_cast<uintptr_t>(raw->header.hDevice)}, .button_index{1}}); }
//							if (rawmouse.usButtonFlags & RI_MOUSE_BUTTON_2_DOWN) { input_manager_ptr->mouse.button_pressed ({.device_handle{reinterpret_cast<uintptr_t>(raw->header.hDevice)}, .button_index{2}}); }
//							if (rawmouse.usButtonFlags & RI_MOUSE_BUTTON_2_UP  ) { input_manager_ptr->mouse.button_released({.device_handle{reinterpret_cast<uintptr_t>(raw->header.hDevice)}, .button_index{2}}); }
//							if (rawmouse.usButtonFlags & RI_MOUSE_BUTTON_3_DOWN) { input_manager_ptr->mouse.button_pressed ({.device_handle{reinterpret_cast<uintptr_t>(raw->header.hDevice)}, .button_index{3}}); }
//							if (rawmouse.usButtonFlags & RI_MOUSE_BUTTON_3_UP  ) { input_manager_ptr->mouse.button_released({.device_handle{reinterpret_cast<uintptr_t>(raw->header.hDevice)}, .button_index{3}}); }
//							if (rawmouse.usButtonFlags & RI_MOUSE_BUTTON_4_DOWN) { input_manager_ptr->mouse.button_pressed ({.device_handle{reinterpret_cast<uintptr_t>(raw->header.hDevice)}, .button_index{4}}); }
//							if (rawmouse.usButtonFlags & RI_MOUSE_BUTTON_4_UP  ) { input_manager_ptr->mouse.button_released({.device_handle{reinterpret_cast<uintptr_t>(raw->header.hDevice)}, .button_index{4}}); }
//							if (rawmouse.usButtonFlags & RI_MOUSE_BUTTON_5_DOWN) { input_manager_ptr->mouse.button_pressed ({.device_handle{reinterpret_cast<uintptr_t>(raw->header.hDevice)}, .button_index{5}}); }
//							if (rawmouse.usButtonFlags & RI_MOUSE_BUTTON_5_UP  ) { input_manager_ptr->mouse.button_released({.device_handle{reinterpret_cast<uintptr_t>(raw->header.hDevice)}, .button_index{5}}); }
//							}
//							break;
//
//							case RIM_TYPEKEYBOARD:
//							{
//							// https://docs.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-rawkeyboard
//							std::cout << "Keyboard handle " << raw->header.hDevice << "\n";
//							const auto& rawkb{raw->data.keyboard};
//
//							if (rawkb.MakeCode == KEYBOARD_OVERRUN_MAKE_CODE || rawkb.VKey == 0xff)//VK__none_
//								{
//								break;
//								}
//
//							try
//								{// https://stackoverflow.com/questions/38584015/using-getkeynametext-with-special-keys
//								uint16_t scanCode = rawkb.MakeCode;
//								scanCode |= (rawkb.Flags & RI_KEY_E0) ? 0xe000 : 0;
//								scanCode |= (rawkb.Flags & RI_KEY_E1) ? 0xe100 : 0;
//
//								constexpr uint16_t c_BreakScanCode{0xe11d}; // emitted on Ctrl+NumLock
//								constexpr uint16_t c_NumLockScanCode{0xe045};
//								constexpr uint16_t c_PauseScanCode{0x0045};
//								// These are special for historical reasons
//								// https://en.wikipedia.org/wiki/Break_key#Modern_keyboards
//								// Without it GetKeyNameTextW API will fail for these keys
//								if (scanCode == c_BreakScanCode) { scanCode = c_PauseScanCode; }
//								else if (scanCode == c_PauseScanCode) { scanCode = c_NumLockScanCode; }
//
//								std::cout << GetScanCodeName(scanCode) << " ";
//
//								if ((rawkb.Flags & RI_KEY_BREAK) == RI_KEY_BREAK) { std::cout << "released\n"; }
//								else if ((rawkb.Flags & RI_KEY_MAKE) == RI_KEY_MAKE) { std::cout << "pressed \n"; }
//								//else if ((rawkb.Flags & RI_KEY_E0   ) == RI_KEY_E0   ) { std::cout << "E0 whatever it means\n"; }
//								//else if ((rawkb.Flags & RI_KEY_E1   ) == RI_KEY_E1   ) { std::cout << "E1 whatever it means\n"; }
//								}
//							catch (const std::system_error& error) { std::cout << error.what() << "\n"; }
//							}
//							break;
//							//case RIM_TYPEMOUSE:
//								//std::cout << "Mouse handle " << raw->header.hDevice << ": " << raw->data.mouse.lLastX << ", " << raw->data.mouse.lLastY << std::endl;
//								//break;
//							}
//						}
//
//					return 0;
//					}
//				}
//				}
//			return std::nullopt;
//			}
//
//	private:
//		utils::observer_ptr<utils::input::manager> input_manager_ptr;
//
//		inline static std::string GetScanCodeName(uint16_t scanCode)
//			{
//			const bool isExtendedKey = (scanCode >> 8) != 0;
//
//			// Some extended keys doesn't work properly with GetKeyNameTextW API
//			if (isExtendedKey)
//				{
//				const uint16_t vkCode = LOWORD(MapVirtualKeyW(scanCode, MAPVK_VSC_TO_VK_EX));
//				switch (vkCode)
//					{
//					case VK_BROWSER_BACK:
//						return "Browser Back";
//					case VK_BROWSER_FORWARD:
//						return "Browser Forward";
//					case VK_BROWSER_REFRESH:
//						return "Browser Refresh";
//					case VK_BROWSER_STOP:
//						return "Browser Stop";
//					case VK_BROWSER_SEARCH:
//						return "Browser Search";
//					case VK_BROWSER_FAVORITES:
//						return "Browser Favorites";
//					case VK_BROWSER_HOME:
//						return "Browser Home";
//					case VK_VOLUME_MUTE:
//						return "Volume Mute";
//					case VK_VOLUME_DOWN:
//						return "Volume Down";
//					case VK_VOLUME_UP:
//						return "Volume Up";
//					case VK_MEDIA_NEXT_TRACK:
//						return "Next Track";
//					case VK_MEDIA_PREV_TRACK:
//						return "Previous Track";
//					case VK_MEDIA_STOP:
//						return "Media Stop";
//					case VK_MEDIA_PLAY_PAUSE:
//						return "Media Play/Pause";
//					case VK_LAUNCH_MAIL:
//						return "Launch Mail";
//					case VK_LAUNCH_MEDIA_SELECT:
//						return "Launch Media Selector";
//					case VK_LAUNCH_APP1:
//						return "Launch App 1";
//					case VK_LAUNCH_APP2:
//						return "Launch App 2";
//					}
//				}
//
//			const LONG lParam = MAKELONG(0, (isExtendedKey ? KF_EXTENDED : 0) | (scanCode & 0xff));
//			wchar_t name[128] = {};
//			size_t nameSize = ::GetKeyNameTextW(lParam, name, sizeof(name));
//
//			std::string ret;
//			std::transform(name, name + nameSize, std::back_inserter(ret), [](wchar_t c) {
//				return (char)c;
//				});
//
//			return ret;
//			}
//	};