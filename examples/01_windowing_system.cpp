#include <iostream>
#include <chrono>

#include <utils/enum.h>

#include <utils/MS/window/window.h>
#include <utils/MS/window/style.h>
#include <utils/MS/window/taskbar.h>
#include <utils/MS/window/regions.h>
#include <utils/MS/window/snap_on_drag.h>

// Let it be recorded to history that I wanted to use '🗔' instead of "window" for the window namespace

#include "examples.h"

void windowing_system_inner()
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
		// Add the style module to the window that allows for transparent, borderless, and borderless transparent windows:
		utils::MS::window::style::create_info
			{
			.transparency{utils::MS::window::style::transparency_t::composition_attribute},
			.borders     {utils::MS::window::style::value_t::disable}
			},
		// Add the resizable_edge that adds a custom resizable edge, needed if you want a resizable window that is also borderless
		utils::MS::window::resizable_edge::create_info
			{
			.thickness{8}
			}
		};

	// Now let's add some modules to the window at any time after window creation.
	// Note: Thanks to my fancy and untested (XD) object_pool container, 
	// you can add modules even while the modules container is being iterated without worries!
	// (the modules container is iterated whenever an event is being processed)
	// Adding a module this way returns an handle through which you can access the module itself.
	// "wmh" is used in variables names as short for "window's module handle"

	// You can create a new module with this method if that module has a create_info struct
	auto wmh_regions{window.emplace_module_from_create_info(utils::MS::window::regions::create_info
		{
		.default_hit_type{utils::MS::window::hit_type::client},
		.regions_data
			{
			utils::MS::window::regions::region_data_t
				{
				.hit_type{utils::MS::window::hit_type::drag},
				.rect{0, 0, 64, 64}
				}
			}
		})};

	// If a module has no create_info struct you can add it this way instead. 
	// Acts as any other std container's emplace, however you should still omit the 1st constructor parameter, the reference to window::base.
	auto wmh_resizable_edge{window.emplace_module<utils::MS::window::resizable_edge>()};
	
	// You can also retrieve an handle to a component that was added on creation or at any other point
	auto wmh_snap_on_drag{window.get_module_handle<utils::MS::window::snap_on_drag>()};

	// Handles can be accessed with a smart-pointer-like interface
	if (wmh_snap_on_drag.has_value())
		{
		wmh_snap_on_drag->snap_max_distance = 32;
		auto& snap_on_drag_ref{*wmh_snap_on_drag};
		snap_on_drag_ref.snap_max_distance = 24;
		}

	// Through an handle you can remove a module from the window. 
	// Note: unlike for modules insertion, removing modules during modules container iteration is NOT safe.
	// (the modules container is iterated whenever an event is being processed)
	// hence if you must remove a module inside the window loop, to that in the while(window.is_open()) body, 
	// but not inside any window procedure or anything that goes through window procedures, like the event system
	wmh_snap_on_drag.reset();

	//querying again for the snap_on_drag module will now return an invalid handle
	auto wmh_snap_on_drag_2{window.get_module_handle<utils::MS::window::snap_on_drag>()};
	assert(!wmh_snap_on_drag.has_value());


	window.show();
	while (window.is_open())
		{
		// wait event will sleep until a new event is received
		window.wait_event();

		// alternatively poll_event will return false if the event queue is empty, for real-time applications like games.
		//while (window.poll_event());
		}
	}

void example::windowing_system()
	{
	try { windowing_system_inner(); }
	catch (const std::system_error& e) { ::MessageBoxA(nullptr, e.what(), "Unhandled Exception", MB_OK | MB_ICONERROR); }
	}