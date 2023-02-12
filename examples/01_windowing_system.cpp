#include <iostream>
#include <chrono>

#include <utils/enum.h>

#include <utils/MS/window/window.h>
#include <utils/MS/window/style.h>
#include <utils/MS/window/regions.h>
#include <utils/MS/window/snap_on_drag.h>
#include <utils/MS/window/input/mouse.h>

// Let it be recorded to history that I wanted to use '🗔' instead of "window" for the window namespace

#include "examples.h"

static void body()
	{
	using namespace utils::output;

	utils::MS::window::initializer window_initializer;

	utils::MS::window::base window
		{
		utils::MS::window::base::create_info{},
		// Simple module to close the window with [ESC]
		close_module::create_info{},
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
			},
		// Input will be explained in the next example, here it's used just to be able to close the window through right-click
		utils::MS::window::input::mouse::create_info{},
		// Simple module to close the window with [ESC]
		close_module::create_info{}
		};

	// Now let's add some modules to the window at any time after window creation.
	// "wm" is used in variables names as short for "window's module"
	// emplace operators return a reference to the emplaced window module.

	// You can create a new module with this method if that module has a create_info struct
	auto& wm_regions{window.emplace_module_from_create_info(utils::MS::window::regions::create_info
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
	// not doing this cause resizable_edge was already in the window.
	//auto& wm_resizable_edge{window.emplace_module<utils::MS::window::resizable_edge>()};
	
	// You can also retrieve a pointer to a component that was added on creation or at any other point
	// get_module_ptr returns nullptr if the module isn't present.
	auto wm_snap_on_drag_ptr{window.get_module_ptr<utils::MS::window::snap_on_drag>()};

	if (wm_snap_on_drag_ptr)
		{
		wm_snap_on_drag_ptr->snap_max_distance = 32;
		}

	// You can also remove modules by type
	window.remove_module<utils::MS::window::snap_on_drag>();

	//querying again for the snap_on_drag module will now return an invalid handle
	auto wm_snap_on_drag_2_ptr{window.get_module_ptr<utils::MS::window::snap_on_drag>()};
	assert(!wm_snap_on_drag_2_ptr);

	// Input will be explained in the next example, here it's used just to be able to close the window through right-click
	window.get_module_ptr<utils::MS::window::input::mouse>()
		->default_mouse.buttons[utils::input::mouse::button_id::right].on_changed.emplace([&window](bool, bool) { window.close(); });

	window.show();
	while (window.is_open())
		{
		window.wait_event();
		}
	}

void example::windowing_system()
	{
	try { body(); }
	catch (const std::system_error& e) { ::MessageBoxA(nullptr, e.what(), "Unhandled Exception", MB_OK | MB_ICONERROR); }
	}