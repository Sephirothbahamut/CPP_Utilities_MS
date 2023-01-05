#include <utils/enum.h>
#include <iostream>

#include "include/utils/win32/window/window.h"
#include "include/utils/win32/window/style.h"
#include "include/utils/win32/window/taskbar.h"
#include "include/utils/win32/window/regions.h"
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
		input_mouse   {*this}
		{
		}
	
	utils::win32::window::style          style         ;
	utils::win32::window::resizable_edge resizable_edge;
	utils::win32::window::regions        regions       ;
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
				.transparency{utils::win32::window::style::transparency_t::composition_attribute},
				.borders     {utils::win32::window::style::value_t       ::disable}
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

		
		window.input_mouse.default_mouse.buttons.left    .on_changed.emplace([](const bool state, const bool) { std::cout << "Mouse button left     " << (state ? "pressed" : "released") << std::endl; });
		window.input_mouse.default_mouse.buttons.right   .on_changed.emplace([](const bool state, const bool) { std::cout << "Mouse button right    " << (state ? "pressed" : "released") << std::endl; });
		window.input_mouse.default_mouse.buttons.middle  .on_changed.emplace([](const bool state, const bool) { std::cout << "Mouse button middle   " << (state ? "pressed" : "released") << std::endl; });
		window.input_mouse.default_mouse.buttons.forward .on_changed.emplace([](const bool state, const bool) { std::cout << "Mouse button forward  " << (state ? "pressed" : "released") << std::endl; });
		window.input_mouse.default_mouse.buttons.backward.on_changed.emplace([](const bool state, const bool) { std::cout << "Mouse button backward " << (state ? "pressed" : "released") << std::endl; });
		

		window.show();
		while (window.is_open())
			{
			while (window.poll_event());
			}
		}
	//catch (const std::system_error& e) { ::MessageBoxA(nullptr, e.what(), "Unhandled Exception", MB_OK | MB_ICONERROR); }
	}