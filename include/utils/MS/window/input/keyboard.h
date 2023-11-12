#pragma once

#include <cctype>
#include <array>
#include <string>
#include <cstdint>
#include <functional>
#include <unordered_map>

#include <utils/enum.h>
#include <utils/memory.h>
#include <utils/math/vec2.h>
#include <utils/math/vec3.h>
#include <utils/input_system/device/keyboard.h>

#include "../../../utf8/utf8.h"

#include "../window.h"

namespace std
	{
	using ::iswblank;
	using ::iswcntrl;
	} // namespace std

// For future textbox shortcuts
// https://learn.microsoft.com/en-us/windows/win32/learnwin32/accelerator-tables

static uint16_t HIDUsageToScanCode(uint8_t usage)
	{
	uint8_t index = usage;

	// 256 bytes
	static constexpr uint8_t usage2scan[] =
		{
			0x00, 0x00, 0x00, 0x00, 0x1e, 0x30, 0x2e, 0x20, 0x12, 0x21, 0x22, 0x23, 0x17, 0x24, 0x25, 0x26,
			0x32, 0x31, 0x18, 0x19, 0x10, 0x13, 0x1f, 0x14, 0x16, 0x2f, 0x11, 0x2d, 0x15, 0x2c, 0x02, 0x03,
			0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x1c, 0x01, 0x0e, 0x0f, 0x39, 0x0c, 0x0d, 0x1a,
			0x1b, 0x2b, 0x2b, 0x27, 0x28, 0x29, 0x33, 0x34, 0x35, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 0x40,
			0x41, 0x42, 0x43, 0x44, 0x57, 0x58, 0xb7, 0x46, 0xc5, 0xd2, 0xc7, 0xc9, 0xd3, 0xcf, 0xd1, 0xcd,
			0xcb, 0xd0, 0xc8, 0x45, 0xb5, 0x37, 0x4a, 0x4e, 0x9c, 0x4f, 0x50, 0x51, 0x4b, 0x4c, 0x4d, 0x47,
			0x48, 0x49, 0x52, 0x53, 0x56, 0xdd, 0xde, 0x59, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b,
			0x6c, 0x6d, 0x6e, 0x76, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x73, 0x70, 0x7d, 0x79, 0x7b, 0x5c, 0x00, 0x00, 0x00,
			0x72, 0x71, 0x78, 0x77, 0x76, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x1d, 0x2a, 0x38, 0xdb, 0x9d, 0x36, 0xb8, 0xdc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
		};

	return ((usage2scan[index] & 0x80) ? 0xe000 : 0) | (usage2scan[index] & 0x7f);
	}

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

			keyboard(window::base& base, const create_info& create_info) : module{base}, input_manager{create_info.input_manager}
				{
				//mouse
				RAWINPUTDEVICE rid_keyboard
					{
					.usUsagePage{0x01},
				//	.usUsage    {0x02},//Mouse
				//	.usUsage    {0x05},//gamepad
					.usUsage    {0x06},//Keyboard
					.dwFlags    {RIDEV_INPUTSINK | RIDEV_NOLEGACY},
					.hwndTarget {base.get_handle()},
					};
				if (!RegisterRawInputDevices(&rid_keyboard, 1, sizeof(rid_keyboard))) { throw last_error("failed to register raw input devices"); }
				}

			std::unordered_map<uintptr_t, input_keyboard> keyboards;

			input_keyboard default_keyboard;

		private:
			utils::input_system::manager& input_manager;

			virtual procedure_result procedure(UINT msg, WPARAM wparam, LPARAM lparam) override
				{
				switch (msg)
					{
					case WM_INPUT  : if(wm_input(wparam, lparam)) { return procedure_result::next(0); }
					}
				
				return procedure_result::next();
				}

			input_keyboard::button_id win32_keycode_to_utils_key_id(UINT keycode)
				{//https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
				switch(keycode)
					{
					  case VK_LBUTTON                /*0x01   */: return input_keyboard::button_id::unknown          ; //Left mouse button
					  case VK_RBUTTON                /*0x02   */: return input_keyboard::button_id::unknown          ; //Right mouse button
					  case VK_CANCEL                 /*0x03   */: return input_keyboard::button_id::unknown          ; //Control - break processing
					  case VK_MBUTTON                /*0x04   */: return input_keyboard::button_id::unknown          ; //Middle mouse button
					  case VK_XBUTTON1               /*0x05   */: return input_keyboard::button_id::unknown          ; //X1 mouse button
					  case VK_XBUTTON2               /*0x06   */: return input_keyboard::button_id::unknown          ; //X2 mouse button
					//case -                         /*0x07   */: return input_keyboard::button_id::unknown          ; //Reserved
					  case VK_BACK                   /*0x08   */: return input_keyboard::button_id::backspace        ; //BACKSPACE key
					  case VK_TAB                    /*0x09   */: return input_keyboard::button_id::tab              ; //TAB key
					//case -                           0x0A-0B  : return input_keyboard::button_id::unknown          ; //Reserved
					  case VK_CLEAR                  /*0x0C   */: return input_keyboard::button_id::unknown          ; //CLEAR key
					  case VK_RETURN                 /*0x0D   */: return input_keyboard::button_id::enter            ; //ENTER key
					//case -                           0x0E-0F  : return input_keyboard::button_id::unknown          ; //Unassigned
					  case VK_SHIFT                  /*0x10   */: return input_keyboard::button_id::shift            ; //SHIFT key
					  case VK_CONTROL                /*0x11   */: return input_keyboard::button_id::ctrl             ; //CTRL key
					  case VK_MENU                   /*0x12   */: return input_keyboard::button_id::alt              ; //ALT key
					  case VK_PAUSE                  /*0x13   */: return input_keyboard::button_id::pause            ; //PAUSE key
					  case VK_CAPITAL                /*0x14   */: return input_keyboard::button_id::caps_lock        ; //CAPS LOCK key
					  case VK_KANA                   /*0x15   */: return input_keyboard::button_id::unknown          ; //IME Kana mode
					//case VK_HANGUL                 /*0x15   */: return input_keyboard::button_id::unknown          ; //IME Hangul mode
					  case VK_IME_ON                 /*0x16   */: return input_keyboard::button_id::unknown          ; //IME On
					  case VK_JUNJA                  /*0x17   */: return input_keyboard::button_id::unknown          ; //IME Junja mode
					  case VK_FINAL                  /*0x18   */: return input_keyboard::button_id::unknown          ; //IME final mode
					  case VK_HANJA                  /*0x19   */: return input_keyboard::button_id::unknown          ; //IME Hanja mode
					//case VK_KANJI                  /*0x19   */: return input_keyboard::button_id::unknown          ; //IME Kanji mode
					  case VK_IME_OFF                /*0x1A   */: return input_keyboard::button_id::unknown          ; //IME Off
					  case VK_ESCAPE                 /*0x1B   */: return input_keyboard::button_id::esc              ; //ESC key
					  case VK_CONVERT                /*0x1C   */: return input_keyboard::button_id::unknown          ; //IME convert
					  case VK_NONCONVERT             /*0x1D   */: return input_keyboard::button_id::unknown          ; //IME nonconvert
					  case VK_ACCEPT                 /*0x1E   */: return input_keyboard::button_id::unknown          ; //IME accept
					  case VK_MODECHANGE             /*0x1F   */: return input_keyboard::button_id::unknown          ; //IME mode change request
					  case VK_SPACE                  /*0x20   */: return input_keyboard::button_id::space            ; //SPACEBAR
					  case VK_PRIOR                  /*0x21   */: return input_keyboard::button_id::page_up          ; //PAGE UP key
					  case VK_NEXT                   /*0x22   */: return input_keyboard::button_id::page_down        ; //PAGE DOWN key
					  case VK_END                    /*0x23   */: return input_keyboard::button_id::end              ; //END key
					  case VK_HOME                   /*0x24   */: return input_keyboard::button_id::home             ; //HOME key
					  case VK_LEFT                   /*0x25   */: return input_keyboard::button_id::arrow_left       ; //LEFT ARROW key
					  case VK_UP                     /*0x26   */: return input_keyboard::button_id::arrow_up         ; //UP ARROW key
					  case VK_RIGHT                  /*0x27   */: return input_keyboard::button_id::arrow_right      ; //RIGHT ARROW key
					  case VK_DOWN                   /*0x28   */: return input_keyboard::button_id::arrow_down       ; //DOWN ARROW key
					  case VK_SELECT                 /*0x29   */: return input_keyboard::button_id::unknown          ; //SELECT key
					  case VK_PRINT                  /*0x2A   */: return input_keyboard::button_id::unknown          ; //PRINT key
					  case VK_EXECUTE                /*0x2B   */: return input_keyboard::button_id::unknown          ; //EXECUTE key
					  case VK_SNAPSHOT               /*0x2C   */: return input_keyboard::button_id::print_screen     ; //PRINT SCREEN key
					  case VK_INSERT                 /*0x2D   */: return input_keyboard::button_id::ins              ; //INS key
					  case VK_DELETE                 /*0x2E   */: return input_keyboard::button_id::del              ; //DEL key
					  case VK_HELP                   /*0x2F   */: return input_keyboard::button_id::unknown          ; //HELP key
					  case                             0x30     : return input_keyboard::button_id::num_0            ; //0 key
					  case                             0x31     : return input_keyboard::button_id::num_1            ; //1 key
					  case                             0x32     : return input_keyboard::button_id::num_2            ; //2 key
					  case                             0x33     : return input_keyboard::button_id::num_3            ; //3 key
					  case                             0x34     : return input_keyboard::button_id::num_4            ; //4 key
					  case                             0x35     : return input_keyboard::button_id::num_5            ; //5 key
					  case                             0x36     : return input_keyboard::button_id::num_6            ; //6 key
					  case                             0x37     : return input_keyboard::button_id::num_7            ; //7 key
					  case                             0x38     : return input_keyboard::button_id::num_8            ; //8 key
					  case                             0x39     : return input_keyboard::button_id::num_9            ; //9 key
					//case -                           0x3A-40  : return input_keyboard::button_id::unknown          ; //Undefined
					  case                             0x41     : return input_keyboard::button_id::a                ; //A key
					  case                             0x42     : return input_keyboard::button_id::b                ; //B key
					  case                             0x43     : return input_keyboard::button_id::c                ; //C key
					  case                             0x44     : return input_keyboard::button_id::d                ; //D key
					  case                             0x45     : return input_keyboard::button_id::e                ; //E key
					  case                             0x46     : return input_keyboard::button_id::f                ; //F key
					  case                             0x47     : return input_keyboard::button_id::g                ; //G key
					  case                             0x48     : return input_keyboard::button_id::h                ; //H key
					  case                             0x49     : return input_keyboard::button_id::i                ; //I key
					  case                             0x4A     : return input_keyboard::button_id::j                ; //J key
					  case                             0x4B     : return input_keyboard::button_id::k                ; //K key
					  case                             0x4C     : return input_keyboard::button_id::l                ; //L key
					  case                             0x4D     : return input_keyboard::button_id::m                ; //M key
					  case                             0x4E     : return input_keyboard::button_id::n                ; //N key
					  case                             0x4F     : return input_keyboard::button_id::o                ; //O key
					  case                             0x50     : return input_keyboard::button_id::p                ; //P key
					  case                             0x51     : return input_keyboard::button_id::q                ; //Q key
					  case                             0x52     : return input_keyboard::button_id::r                ; //R key
					  case                             0x53     : return input_keyboard::button_id::s                ; //S key
					  case                             0x54     : return input_keyboard::button_id::t                ; //T key
					  case                             0x55     : return input_keyboard::button_id::u                ; //U key
					  case                             0x56     : return input_keyboard::button_id::v                ; //V key
					  case                             0x57     : return input_keyboard::button_id::w                ; //W key
					  case                             0x58     : return input_keyboard::button_id::x                ; //X key
					  case                             0x59     : return input_keyboard::button_id::y                ; //Y key
					  case                             0x5A     : return input_keyboard::button_id::z                ; //Z key
					  case VK_LWIN                   /*0x5B*/   : return input_keyboard::button_id::win_left         ; //Left Windows key
					  case VK_RWIN                   /*0x5C*/   : return input_keyboard::button_id::win_right        ; //Right Windows key
					  case VK_APPS                   /*0x5D*/   : return input_keyboard::button_id::unknown          ; //Applications key
					//case -                           0x5E     : return input_keyboard::button_id::unknown          ; //Reserved
					  case VK_SLEEP                  /*0x5F*/   : return input_keyboard::button_id::unknown          ; //Computer Sleep key
					  case VK_NUMPAD0                /*0x60*/   : return input_keyboard::button_id::numpad_0         ; //Numeric keypad 0 key
					  case VK_NUMPAD1                /*0x61*/   : return input_keyboard::button_id::numpad_1         ; //Numeric keypad 1 key
					  case VK_NUMPAD2                /*0x62*/   : return input_keyboard::button_id::numpad_2         ; //Numeric keypad 2 key
					  case VK_NUMPAD3                /*0x63*/   : return input_keyboard::button_id::numpad_3         ; //Numeric keypad 3 key
					  case VK_NUMPAD4                /*0x64*/   : return input_keyboard::button_id::numpad_4         ; //Numeric keypad 4 key
					  case VK_NUMPAD5                /*0x65*/   : return input_keyboard::button_id::numpad_5         ; //Numeric keypad 5 key
					  case VK_NUMPAD6                /*0x66*/   : return input_keyboard::button_id::numpad_6         ; //Numeric keypad 6 key
					  case VK_NUMPAD7                /*0x67*/   : return input_keyboard::button_id::numpad_7         ; //Numeric keypad 7 key
					  case VK_NUMPAD8                /*0x68*/   : return input_keyboard::button_id::numpad_8         ; //Numeric keypad 8 key
					  case VK_NUMPAD9                /*0x69*/   : return input_keyboard::button_id::numpad_9         ; //Numeric keypad 9 key
					  case VK_MULTIPLY               /*0x6A*/   : return input_keyboard::button_id::numpad_multiply  ; //Multiply key
					  case VK_ADD                    /*0x6B*/   : return input_keyboard::button_id::numpad_add       ; //Add key
					  case VK_SEPARATOR              /*0x6C*/   : return input_keyboard::button_id::numpad_separator ; //Separator key //obscure thing in few keyboards, ssplits the numpad + key in 2 1unit keys
					  case VK_SUBTRACT               /*0x6D*/   : return input_keyboard::button_id::numpad_subtract  ; //Subtract key
					  case VK_DECIMAL                /*0x6E*/   : return input_keyboard::button_id::numpad_decimal   ; //Decimal key
					  case VK_DIVIDE                 /*0x6F*/   : return input_keyboard::button_id::numpad_divide    ; //Divide key
					  case VK_F1                     /*0x70*/   : return input_keyboard::button_id::F1               ; //F1 key
					  case VK_F2                     /*0x71*/   : return input_keyboard::button_id::F2               ; //F2 key
					  case VK_F3                     /*0x72*/   : return input_keyboard::button_id::F3               ; //F3 key
					  case VK_F4                     /*0x73*/   : return input_keyboard::button_id::F4               ; //F4 key
					  case VK_F5                     /*0x74*/   : return input_keyboard::button_id::F5               ; //F5 key
					  case VK_F6                     /*0x75*/   : return input_keyboard::button_id::F6               ; //F6 key
					  case VK_F7                     /*0x76*/   : return input_keyboard::button_id::F7               ; //F7 key
					  case VK_F8                     /*0x77*/   : return input_keyboard::button_id::F8               ; //F8 key
					  case VK_F9                     /*0x78*/   : return input_keyboard::button_id::F9               ; //F9 key
					  case VK_F10                    /*0x79*/   : return input_keyboard::button_id::F10              ; //F10 key
					  case VK_F11                    /*0x7A*/   : return input_keyboard::button_id::F11              ; //F11 key
					  case VK_F12                    /*0x7B*/   : return input_keyboard::button_id::F12              ; //F12 key
					  case VK_F13                    /*0x7C*/   : return input_keyboard::button_id::F13              ; //F13 key
					  case VK_F14                    /*0x7D*/   : return input_keyboard::button_id::F14              ; //F14 key
					  case VK_F15                    /*0x7E*/   : return input_keyboard::button_id::F15              ; //F15 key
					  case VK_F16                    /*0x7F*/   : return input_keyboard::button_id::F16              ; //F16 key
					  case VK_F17                    /*0x80*/   : return input_keyboard::button_id::F17              ; //F17 key
					  case VK_F18                    /*0x81*/   : return input_keyboard::button_id::F18              ; //F18 key
					  case VK_F19                    /*0x82*/   : return input_keyboard::button_id::F19              ; //F19 key
					  case VK_F20                    /*0x83*/   : return input_keyboard::button_id::F20              ; //F20 key
					  case VK_F21                    /*0x84*/   : return input_keyboard::button_id::F21              ; //F21 key
					  case VK_F22                    /*0x85*/   : return input_keyboard::button_id::F22              ; //F22 key
					  case VK_F23                    /*0x86*/   : return input_keyboard::button_id::F23              ; //F23 key
					  case VK_F24                    /*0x87*/   : return input_keyboard::button_id::F24              ; //F24 key
					//case -                           0x88     : return input_keyboard::button_id::unknown          ; - 8F //Reserved
					  case VK_NUMLOCK                /*0x90*/   : return input_keyboard::button_id::num_lock         ; //NUM LOCK key
					  case VK_SCROLL                 /*0x91*/   : return input_keyboard::button_id::scroll_lock      ; //SCROLL LOCK key
					//case -                           0x92     : return input_keyboard::button_id::unknown          ; - 96 //OEM specific
					//case -                           0x97     : return input_keyboard::button_id::unknown          ; - 9F //Unassigned
					  case VK_LSHIFT                 /*0xA0*/   : return input_keyboard::button_id::shift_left       ; //Left SHIFT key
					  case VK_RSHIFT                 /*0xA1*/   : return input_keyboard::button_id::shift_right      ; //Right SHIFT key
					  case VK_LCONTROL               /*0xA2*/   : return input_keyboard::button_id::ctrl_left        ; //Left CONTROL key
					  case VK_RCONTROL               /*0xA3*/   : return input_keyboard::button_id::ctrl_right       ; //Right CONTROL key
					  case VK_LMENU                  /*0xA4*/   : return input_keyboard::button_id::alt_left         ; //Left ALT key
					  case VK_RMENU                  /*0xA5*/   : return input_keyboard::button_id::alt_right        ; //Right ALT key
					  case VK_BROWSER_BACK           /*0xA6*/   : return input_keyboard::button_id::unknown          ; //Browser Back key
					  case VK_BROWSER_FORWARD        /*0xA7*/   : return input_keyboard::button_id::unknown          ; //Browser Forward key
					  case VK_BROWSER_REFRESH        /*0xA8*/   : return input_keyboard::button_id::unknown          ; //Browser Refresh key
					  case VK_BROWSER_STOP           /*0xA9*/   : return input_keyboard::button_id::unknown          ; //Browser Stop key
					  case VK_BROWSER_SEARCH         /*0xAA*/   : return input_keyboard::button_id::unknown          ; //Browser Search key
					  case VK_BROWSER_FAVORITES      /*0xAB*/   : return input_keyboard::button_id::unknown          ; //Browser Favorites key
					  case VK_BROWSER_HOME           /*0xAC*/   : return input_keyboard::button_id::unknown          ; //Browser Start and Home key
					  case VK_VOLUME_MUTE            /*0xAD*/   : return input_keyboard::button_id::media_mute       ; //Volume Mute key
					  case VK_VOLUME_DOWN            /*0xAE*/   : return input_keyboard::button_id::media_volume_up  ; //Volume Down key
					  case VK_VOLUME_UP              /*0xAF*/   : return input_keyboard::button_id::media_volume_down; //Volume Up key
					  case VK_MEDIA_NEXT_TRACK       /*0xB0*/   : return input_keyboard::button_id::media_next       ; //Next Track key
					  case VK_MEDIA_PREV_TRACK       /*0xB1*/   : return input_keyboard::button_id::media_previous   ; //Previous Track key
					  case VK_MEDIA_STOP             /*0xB2*/   : return input_keyboard::button_id::media_stop       ; //Stop Media key
					  case VK_MEDIA_PLAY_PAUSE       /*0xB3*/   : return input_keyboard::button_id::media_pause      ; //Play / Pause Media key
					  case VK_LAUNCH_MAIL            /*0xB4*/   : return input_keyboard::button_id::unknown          ; //Start Mail key
					  case VK_LAUNCH_MEDIA_SELECT    /*0xB5*/   : return input_keyboard::button_id::unknown          ; //Select Media key
					  case VK_LAUNCH_APP1            /*0xB6*/   : return input_keyboard::button_id::unknown          ; //Start Application 1 key
					  case VK_LAUNCH_APP2            /*0xB7*/   : return input_keyboard::button_id::unknown          ; //Start Application 2 key
					//case -                           0xB8 - B9: return input_keyboard::button_id::unknown          ; //Reserved
					  case VK_OEM_1                  /*0xBA*/   : return input_keyboard::button_id::oem_1            ; //Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the ;: key
					  case VK_OEM_PLUS               /*0xBB*/   : return input_keyboard::button_id::add              ; //For any country / region, the + key
					  case VK_OEM_COMMA              /*0xBC*/   : return input_keyboard::button_id::comma            ; //For any country / region, the , key
					  case VK_OEM_MINUS              /*0xBD*/   : return input_keyboard::button_id::subtract         ; //For any country / region, the - key
					  case VK_OEM_PERIOD             /*0xBE*/   : return input_keyboard::button_id::period           ; //For any country / region, the . key
					  case VK_OEM_2                  /*0xBF*/   : return input_keyboard::button_id::oem_2            ; //Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the /? key
					  case VK_OEM_3                  /*0xC0*/   : return input_keyboard::button_id::oem_3            ; //Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the `~ key
					//case -                           0xC1     : return input_keyboard::button_id::unknown          ; - DA //Reserved
					  case VK_OEM_4                  /*0xDB*/   : return input_keyboard::button_id::oem_4            ; //Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the [{ key
					  case VK_OEM_5                  /*0xDC*/   : return input_keyboard::button_id::oem_5            ; //Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the \| key
					  case VK_OEM_6                  /*0xDD*/   : return input_keyboard::button_id::oem_6            ; //Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the ]} key
					  case VK_OEM_7                  /*0xDE*/   : return input_keyboard::button_id::oem_7            ; //Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '" key
					  case VK_OEM_8                  /*0xDF*/   : return input_keyboard::button_id::oem_8            ; //Used for miscellaneous characters; it can vary by keyboard.
					//case -                           0xE0     : return input_keyboard::button_id::unknown          ; //Reserved
					//case -                           0xE1     : return input_keyboard::button_id::unknown          ; //OEM specific
					  case VK_OEM_102                /*0xE2*/   : return input_keyboard::button_id::oem_102          ; //The <> keys on the US standard keyboard, or the \| key on the non - US 102 - key keyboard
					//case -                           0xE3     : return input_keyboard::button_id::unknown          ; - E4 //OEM specific
					  case VK_PROCESSKEY             /*0xE5*/   : return input_keyboard::button_id::unknown          ; //IME PROCESS key
					//case -                           0xE6     : return input_keyboard::button_id::unknown          ; //OEM specific
					  case VK_PACKET                 /*0xE7*/   : return input_keyboard::button_id::unknown          ; //Used to pass Unicode characters as if they were keystrokes.The VK_PACKET key is the low word of a 32 - bit Virtual Key value used for non - keyboard input methods.For more information, see Remark in KEYBDINPUT, SendInput, WM_KEYDOWN, and WM_KEYUP
					//case -                           0xE8     : return input_keyboard::button_id::unknown          ; //Unassigned
					//case -                           0xE9     : return input_keyboard::button_id::unknown          ; - F5 //OEM specific
					  case VK_ATTN                   /*0xF6*/   : return input_keyboard::button_id::unknown          ; //Attn key
					  case VK_CRSEL                  /*0xF7*/   : return input_keyboard::button_id::unknown          ; //CrSel key
					  case VK_EXSEL                  /*0xF8*/   : return input_keyboard::button_id::unknown          ; //ExSel key
					  case VK_EREOF                  /*0xF9*/   : return input_keyboard::button_id::unknown          ; //Erase EOF key
					  case VK_PLAY                   /*0xFA*/   : return input_keyboard::button_id::unknown          ; //Play key
					  case VK_ZOOM                   /*0xFB*/   : return input_keyboard::button_id::unknown          ; //Zoom key
					  case VK_NONAME                 /*0xFC*/   : return input_keyboard::button_id::unknown          ; //Reserved
					  case VK_PA1                    /*0xFD*/   : return input_keyboard::button_id::unknown          ; //PA1 key
					  case VK_OEM_CLEAR              /*0xFE*/   : return input_keyboard::button_id::unknown          ; //Clear key

					  default: return input_keyboard::button_id::unknown;
					};
				}

			void default_process_digital(const input_keyboard::button_id& id, bool new_state) noexcept
				{
				default_keyboard.digital.change(input_manager, id, new_state);
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
					if (raw->header.dwType != RIM_TYPEKEYBOARD) { return false; }

					auto device_handle{reinterpret_cast<uintptr_t>(raw->header.hDevice)};

					auto result{keyboards.emplace(std::piecewise_construct, std::make_tuple(device_handle), std::make_tuple())};
					auto& keyboard{result.first->second};

					// https://stackoverflow.com/questions/33610124/how-to-convert-from-a-hid-usage-id-to-a-virtual-key-code-in-windows-os
					// https://learn.microsoft.com/en-us/windows/win32/inputdev/about-keyboard-input#scan-codes
					// https://blog.molecular-matters.com/2011/09/05/properly-handling-keyboard-input/
					
					const auto& raw_keyboard{raw->data.keyboard};

					const bool global{GET_RAWINPUT_CODE_WPARAM(wparam) == RIM_INPUTSINK};

					bool keyUp = (raw_keyboard.Flags & RI_KEY_BREAK) == RI_KEY_BREAK;
					key_codes key_codes{raw_keyboard};

					if (keyUp)
						{
						std::cout << "_________________________________\n";
						std::cout << "Post-process - scan: " << key_codes.scan_code << " vk: " << key_codes.virtual_key << "\n";
						std::string scanCodeName = GetScanCodeName(key_codes.scan_code);
						std::cout << scanCodeName << std::endl;
						}
					}

				return true;
				}

			struct key_codes 
				{
				key_codes(const RAWKEYBOARD& raw_keyboard) : virtual_key{raw_keyboard.VKey}, scan_code {raw_keyboard.MakeCode}
					{
					const uint16_t in_virtual_key = raw_keyboard.VKey;
					const uint16_t in_scan_code   = raw_keyboard.MakeCode;
					const uint16_t in_flags       = raw_keyboard.Flags;

					if (in_scan_code == KEYBOARD_OVERRUN_MAKE_CODE || in_virtual_key == 0xff/*VK__none_*/)
						{
						scan_code = virtual_key = 0xff;
						}

					scan_code |= (in_flags & RI_KEY_E0) ? 0xe000 : 0;
					scan_code |= (in_flags & RI_KEY_E1) ? 0xe100 : 0;

					constexpr uint16_t c_BreakScanCode   = 0xe11d; // emitted on Ctrl+NumLock
					constexpr uint16_t c_NumLockScanCode = 0xe045;
					constexpr uint16_t c_PauseScanCode   = 0x0045;

					// These are special for historical reasons
					// https://en.wikipedia.org/wiki/Break_key#Modern_keyboards
					// Without it GetKeyNameTextW API will fail for these keys
					     if (scan_code == c_BreakScanCode) { scan_code = c_PauseScanCode  ; }
					else if (scan_code == c_PauseScanCode) { scan_code = c_NumLockScanCode; }
					}
				uint16_t virtual_key; uint16_t scan_code; 
				};

				// Clears keyboard buffer
				// Needed to avoid side effects on other calls to ToUnicode API
				// http://archives.miloush.net/michkap/archive/2007/10/27/5717859.html
				inline void ClearKeyboardBuffer(uint16_t vkCode)
					{
					std::array<wchar_t, 10> chars{};
					const uint16_t scanCode = LOWORD(::MapVirtualKeyW(vkCode, MAPVK_VK_TO_VSC_EX));
					int count = 0;
					do
						{
						count = ::ToUnicode(vkCode, scanCode, nullptr, chars.data(), static_cast<int>(chars.size()), 0);
						}
					while (count < 0);
					}

				std::string GetStringFromKeyPress(uint16_t scanCode)
					{
					std::array<wchar_t, 10> chars{};
					const uint16_t vkCode = LOWORD(::MapVirtualKeyW(scanCode, MAPVK_VSC_TO_VK_EX));
					std::array<uint8_t, 256> keyboardState{};

					// Turn on CapsLock to return capital letters
					keyboardState[VK_CAPITAL] = 0b00000001;

					ClearKeyboardBuffer(VK_DECIMAL);

					// For some keyboard layouts ToUnicode() API call can produce multiple chars: UTF-16 surrogate pairs or ligatures.
					// Such layouts are listed here: https://kbdlayout.info/features/ligatures
					int count = ::ToUnicode(vkCode, scanCode, keyboardState.data(), chars.data(), static_cast<int>(chars.size()), 0);

					ClearKeyboardBuffer(VK_DECIMAL);

					return utf8::narrow(chars.data(), std::abs(count));
					}

				std::string GetScanCodeName(uint16_t scanCode)
					{
					static struct
						{
						uint16_t scanCode;
						const char* keyText;
						} mediaKeys[] =
							{
								{ 0xe010, "Previous Track"}, // VK_MEDIA_PREV_TRACK
								{ 0xe019, "Next Track"}, // VK_MEDIA_NEXT_TRACK
								{ 0xe020, "Volume Mute"}, // VK_VOLUME_MUTE
								{ 0xe021, "Launch App 2"}, // VK_LAUNCH_APP2
								{ 0xe022, "Media Play/Pause"}, // VK_MEDIA_PLAY_PAUSE
								{ 0xe024, "Media Stop"},// VK_MEDIA_STOP
								{ 0xe02e, "Volume Down"}, // VK_VOLUME_DOWN
								{ 0xe030, "Volume Up"}, // VK_VOLUME_UP
								{ 0xe032, "Browser Home"}, // VK_BROWSER_HOME
								{ 0xe05e, "System Power"}, // System Power (no VK code)
								{ 0xe05f, "System Sleep"}, // VK_SLEEP
								{ 0xe063, "System Wake"}, // System Wake (no VK code)
								{ 0xe065, "Browser Search"}, // VK_BROWSER_SEARCH
								{ 0xe066, "Browser Favorites"}, // VK_BROWSER_FAVORITES
								{ 0xe067, "Browser Refresh"}, // VK_BROWSER_REFRESH
								{ 0xe068, "Browser Stop"}, // VK_BROWSER_STOP
								{ 0xe069, "Browser Forward"}, // VK_BROWSER_FORWARD
								{ 0xe06a, "Browser Back"}, // VK_BROWSER_BACK
								{ 0xe06b, "Launch App 1"}, // VK_LAUNCH_APP1
								{ 0xe06c, "Launch Mail"}, // VK_LAUNCH_MAIL
								{ 0xe06d, "Launch Media Selector"} // VK_LAUNCH_MEDIA_SELECT
							};

						auto it = std::find_if(std::begin(mediaKeys), std::end(mediaKeys),
							[scanCode](auto& key) { return key.scanCode == scanCode; });
						if (it != std::end(mediaKeys))
							return it->keyText;

						std::string keyText = GetStringFromKeyPress(scanCode);
						std::wstring keyTextWide = utf8::widen(keyText);
						if (!keyTextWide.empty() && !std::iswblank(keyTextWide[0]) && !std::iswcntrl(keyTextWide[0]))
							{
							return keyText;
							}

						std::array<wchar_t, 128> buffer{};
						const LPARAM lParam = MAKELPARAM(0, ((scanCode & 0xff00) ? KF_EXTENDED : 0) | (scanCode & 0xff));
						int count = ::GetKeyNameTextW(static_cast<LONG>(lParam), buffer.data(), static_cast<int>(buffer.size()));

						return utf8::narrow(buffer.data(), count);
					}
		};

	}