#include <utils/enum.h>
#include <iostream>
#include <chrono>

#include "include/utils/win32/window/window.h"
#include "include/utils/win32/window/style.h"
#include "include/utils/win32/window/taskbar.h"
#include "include/utils/win32/window/regions.h"
#include "include/utils/win32/window/snap_on_drag.h"
#include "include/utils/win32/window/input/mouse.h"
// Let it be recorded to history that I wanted to use '🗔' instead of "window" for the window namespace

std::vector<RAWINPUTDEVICELIST> GetRawInputDevices()
	{
	UINT deviceCount = 10; // initial guess, must be nonzero
	std::vector<RAWINPUTDEVICELIST> devices(deviceCount);
	while (deviceCount != 0) {
		UINT actualDeviceCount = GetRawInputDeviceList(
			devices.data(), &deviceCount,
			sizeof(devices[0]));
		if (actualDeviceCount != (UINT)-1) {
			devices.resize(actualDeviceCount);
			return devices;
			}
		DWORD error = GetLastError();
		if (error != ERROR_INSUFFICIENT_BUFFER) {
			std::terminate(); // throw something
			}
		devices.resize(deviceCount);
		}
	}

struct window_sample : utils::win32::window::base
	{
	struct create_info
		{
		utils::win32::window::base::create_info           base;
		utils::win32::window::style::create_info          style;
		utils::win32::window::resizable_edge::create_info resizable_edge;
		utils::win32::window::regions::create_info        regions;
		};

	window_sample(const create_info& create_info) :
		utils::win32::window::base{create_info.style.adjust_base_create_info(create_info.base)},
		style         {*this, create_info.style},
		resizable_edge{*this, create_info.resizable_edge},
		regions       {*this, create_info.regions},
		snap_on_drag  {*this},
		input_mouse   {*this}
		{
		}
	
	utils::win32::window::style          style         ;
	utils::win32::window::resizable_edge resizable_edge;
	utils::win32::window::regions        regions       ;
	utils::win32::window::snap_on_drag   snap_on_drag  ;
	utils::win32::window::input::mouse   input_mouse   ;
	};


int main()
	{
	for(const auto& device : GetRawInputDevices())
		{
		switch (device.dwType)
			{
			case RIM_TYPEKEYBOARD: std::cout << "Keyboard "; break;
			case RIM_TYPEMOUSE   : std::cout << "Mouse    "; break;
			case RIM_TYPEHID     : std::cout << "HID      "; break;
			default:               std::cout << "Other    "; break;
			}
		std::cout << "(" << reinterpret_cast<uintptr_t>(device.hDevice) << ")" << std::endl;
		}

	//try
		{
		using namespace utils::output;

		utils::win32::window::initializer window_initializer;

		window_sample window{window_sample::create_info{
			.base
				{
				.title{L"George G. Goof"},
				//.position{{10, 60}},
				//.size{{800, 600}}
				},
			.style
				{
				//.transparency{utils::win32::window::style::transparency_t::composition_attribute},
				//.borders     {utils::win32::window::style::value_t       ::disable}
				},
			.resizable_edge
				{
				.thickness{8}
				},
			.regions
				{
				.default_hit_type{utils::win32::window::hit_type::client},
				.regions_data
					{
					utils::win32::window::regions::region_data_t
						{
						.hit_type{utils::win32::window::hit_type::drag},
						.rect{0, 0, 64, 64}
						}
					}
				},
			}};


		window.input_mouse.default_mouse.buttons.on_changed.emplace([&window](const utils::input::mouse::button_id& id, const bool& state, const bool&) 
			{
			std::cout << "Mouse button " << utils::enums::enum_name<utils::input::mouse::button_id>(id) << " " << (state ? "pressed" : "released") << std::endl; 

			utils::win32::window::rect_t rect;
			rect.position() = {32, 32};
			rect.size()     = {256, 256};
			switch (id)
				{
				case utils::input::mouse::button_id::left:
					window.window_rect = rect;
					break;
				case utils::input::mouse::button_id::right:
					window.client_rect = rect;
					break;
				}

			std::cout << "window rect: " << window.window_rect << std::endl;
			std::cout << "client_rect: " << window.client_rect << std::endl;
			std::cout << "window size: " << static_cast<utils::math::vec2l>(window.window_rect.size()) << std::endl;
			std::cout << "client size: " << static_cast<utils::math::vec2l>(window.client_rect.size()) << std::endl;
			std::cout << "___________________________________" << std::endl;
			});

		using huq = utils::input::digital::callback_handle_unique;

		huq handle_left    {window.input_mouse.default_mouse.buttons[utils::input::mouse::button_id::left    ].on_changed.make_unique([&handle_left    ](const bool& state, const bool&)
			{
			std::cout << "Self-removing mouse button left     " << (state ? "pressed" : "released") << std::endl; 
			handle_left    .reset();
			})};
		huq handle_right   {window.input_mouse.default_mouse.buttons[utils::input::mouse::button_id::right   ].on_changed.make_unique([&handle_right   ](const bool state, const bool)
			{
			std::cout << "Self-removing mouse button right    " << (state ? "pressed" : "released") << std::endl;
			handle_right   .reset();
			})};
		huq handle_middle  {window.input_mouse.default_mouse.buttons[utils::input::mouse::button_id::middle  ].on_changed.make_unique([&handle_middle  ](const bool state, const bool)
			{
			std::cout << "Self-removing mouse button middle   " << (state ? "pressed" : "released") << std::endl;
			handle_middle  .reset();
			})};
		huq handle_forward {window.input_mouse.default_mouse.buttons[utils::input::mouse::button_id::forward ].on_changed.make_unique([&handle_forward ](const bool state, const bool)
			{
			std::cout << "Self-removing mouse button forward  " << (state ? "pressed" : "released") << std::endl;
			handle_forward .reset();
			})};
		huq handle_backward{window.input_mouse.default_mouse.buttons[utils::input::mouse::button_id::backward].on_changed.make_unique([&handle_backward](const bool state, const bool)
			{
			std::cout << "Self-removing mouse button backward " << (state ? "pressed" : "released") << std::endl;
			handle_backward.reset();

			//Busy loop to force debugger hot path
			auto tp{std::chrono::steady_clock::now()};
			while ((std::chrono::steady_clock::now() - tp) < std::chrono::seconds(10));
			})};

		window.input_mouse.default_mouse.position.on_changed.emplace([](const utils::math::vec2l& new_position, const utils::math::vec2l&)
			{
			using namespace utils::output;
			std::cout << "Mouse moved to " << new_position << std::endl;
			});

		window.show();
		while (window.is_open())
			{
			while (window.poll_event());
			}
		}
	//catch (const std::system_error& e) { ::MessageBoxA(nullptr, e.what(), "Unhandled Exception", MB_OK | MB_ICONERROR); }
	}