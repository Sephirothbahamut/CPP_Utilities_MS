#include <iostream>
#include <chrono>

#include <utils/enum.h>

#include <utils/MS/window/window.h>
#include <utils/MS/window/style.h>
#include <utils/MS/window/taskbar.h>
#include <utils/MS/window/regions.h>
#include <utils/MS/window/snap_on_drag.h>
#include <utils/MS/window/input/mouse.h>

#include "examples.h"

void input_system_inner()
	{
	using namespace utils::output;

	utils::MS::window::initializer window_initializer;

	utils::MS::window::base window
		{
		utils::MS::window::base::create_info{.size{{256u, 128u}}},
		// Create a mouse window module. This will bridge between the OS's input and my input system
		utils::MS::window::input::mouse::create_info{}
		};

	// Get the handle of the mouse input module.
	auto wmh_mouse{window.get_module_handle<utils::MS::window::input::mouse>()};

	// Default mouse is your usual cursor. 
	// You can also add custm virtual mice, to support multiple mice as separate inputs
	// ...well you could if that wasn't still under construction, but achieving that is one objective
	// of my input system
	auto& default_mouse{wmh_mouse->default_mouse};

	// register_debug_callbacks() will add callbacks to the mouse which write to the console
	// any input the mouse is receiving.
	// it returns an unique_ptr-like object which is responsible for the lifetime of those callbacks.
	// if that object is destroyed, the debug callbacks are removed.
	auto debug_callbacks_handle{default_mouse.register_debug_callbacks()};

	// Now let's add something fancier

	// Register an event that unregisters the debug callbacks when right click is pressed
	// Note: the last parameter is the previous state. In general it's useless for digital inputs like mouse buttons,
	// as it's always going to be !state. However it can be useful for analog ones.
	// Since we're not using it at all, I'm leaving it unnamed.
	default_mouse.buttons[utils::input::mouse::button_id::right].on_changed.emplace([&debug_callbacks_handle](bool state, bool)
		{
		debug_callbacks_handle.unregister();
		});

	// You can also react to buttons changes in general, and receive the button id as a parameter instead.
	// Useful if you'd rather switch on all buttons
	default_mouse.buttons.on_changed.emplace([&window](const utils::input::mouse::button_id& id, bool state, bool)
		{
		if (!state) { return; }

		utils::MS::window::rect_t moved{window.window_rect};

		//move the window up or down depending on the pressed button
		switch (id)
			{
			case utils::input::mouse::button_id::backward: moved.y() -= 10; break;
			case utils::input::mouse::button_id::forward : moved.y() += 10; break;
			}

		window.window_rect = moved;
		});
		
	window.show();
	while (window.is_open())
		{
		window.wait_event();
		}
	}

void example::input_system()
	{
	try { input_system_inner(); }
	catch (const std::system_error& e) { ::MessageBoxA(nullptr, e.what(), "Unhandled Exception", MB_OK | MB_ICONERROR); }
	}