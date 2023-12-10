#include <iostream>
#include <chrono>

#include <utils/enum.h>

#include <utils/MS/window/window.h>
#include <utils/MS/windows.h>

// Let it be recorded to history that I wanted to use '🗔' instead of "window" for the window namespace

#include "examples.h"

static void body()
	{
	using namespace utils::output;

	// One window_initializer instance must exist before creating one or multiple windows.
	utils::MS::window::initializer window_initializer;

	// Create a window, and add some modules on window creation.
	utils::MS::window::base window
		{
		// Base parameters to create a window
		utils::MS::window::base::create_info
			{
			.title{L"Donald Fauntleroy Duck"}
			},
		// Ignore this, modules are explained in the next example
		close_module::create_info{}
		};

	// needed to actually open the window
	window.show();
	while (window.is_open())
		{
		// wait event will sleep until a new event is received
		window.wait_event();

		// alternatively poll_event will return false if the event queue is empty, for real-time applications like games.
		// while (window.poll_event());
		}
	}

void example::base_window()
	{
	try { body(); }
	catch (const std::system_error& e) { ::MessageBoxA(nullptr, e.what(), "Unhandled Exception", MB_OK | MB_ICONERROR); }
	}