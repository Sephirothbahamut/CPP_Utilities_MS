#include <iostream>
#include <chrono>

#include <utils/enum.h>

#include <utils/MS/window/window.h>
#include <utils/MS/window/input/mouse.h>

#include "examples.h"

static void body()
	{
	using namespace utils::output;

	utils::input_system::manager manager_input;

	utils::MS::window::initializer window_initializer;

	utils::MS::window::base window
		{
		utils::MS::window::base::create_info{.size{{256u, 128u}}},
		close_module::create_info{},
		// Create a mouse window module. This will bridge between the OS's input and my input system
		utils::MS::window::input::mouse::create_info{manager_input}
		};

	// Get the mouse input module.
	auto& wm_mouse{*window.get_module_ptr<utils::MS::window::input::mouse>()};

	// Default mouse is your usual cursor. 
	// You can also add custm virtual mice, to support multiple mice as separate inputs
	// ...well you could if that wasn't still under construction, but achieving that is one objective
	// of my input system
	auto& default_mouse{wm_mouse.default_mouse};

	// register_debug_callbacks() will add callbacks to the mouse which write to the console
	// any input the mouse is receiving.
	// it returns an unique_ptr-like object which is responsible for the lifetime of those callbacks.
	// if that object is destroyed, the debug callbacks are removed.
	utils::input_system::device::mouse::debug_callbacks mouse_debug_callbacks{default_mouse};

	// Now let's add something fancier

	// Register an event that unregisters the debug callbacks when right click is pressed
	// Note: the last parameter is the previous state. In general it's useless for digital inputs like mouse buttons,
	// as it's always going to be !state. However it can be useful for analog ones.
	// Since we're not using it at all, I'm leaving it unnamed.

	//struct event_t: utils::input_system::event::base
	//	{
	//	event_t(std::unique_ptr<utils::input_system::mapping::button::base>&& mapping) : mapping{std::move(mapping)} 
	//		{
	//		this->mapping->map(*this);
	//		}
	//	std::unique_ptr<utils::input_system::mapping::button::base> mapping;
	//	virtual utils::input_system::on_completion operator()() final override
	//		{
	//		if (mapping.get()->value().current && !mapping.get()->value().previous)
	//			{
	//			std::cout << "pippo begin" << std::endl;
	//			//mapping.get()->value().change(mapping.get()->value().current); //TODO
	//			return utils::input_system::on_completion::keep;
	//			}
	//		if (!mapping.get()->value().current && mapping.get()->value().previous)
	//			{
	//			std::cout << "pippo end" << std::endl;
	//			return utils::input_system::on_completion::remove;
	//			}
	//
	//		std::cout << "pippo" << std::endl;
	//		return utils::input_system::on_completion::keep;
	//		}
	//	};
	//event_t event{std::make_unique<utils::input_system::mapping::button::from_digital>(default_mouse.digital[utils::input_system::device::mouse::button_id::right])};
	//auto& tmp{default_mouse.digital[utils::input_system::device::mouse::button_id::right]};
	//auto event{[&tmp]() 
	//	{
	//	//TODO unregister debug callbacks
	//	std::cout << "pippo" << std::endl;
	//	}};
	//tmp.map(event);
	
	////////////////////////////////////////////////////////////////////default_mouse.buttons[utils::input::mouse::button_id::right].on_changed.emplace([&debug_callbacks_handle](bool state, bool)
	////////////////////////////////////////////////////////////////////	{
	////////////////////////////////////////////////////////////////////	debug_callbacks_handle.unregister();
	////////////////////////////////////////////////////////////////////	});

	// You can also react to buttons changes in general, and receive the button id as a parameter instead.
	// Useful if you'd rather switch on all buttons
	/////////////////////////////////////////////////////////////////////default_mouse.buttons.on_changed.emplace([&window](const utils::input::mouse::button_id& id, bool state, bool)
	/////////////////////////////////////////////////////////////////////	{
	/////////////////////////////////////////////////////////////////////	if (!state) { return; }
	/////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////	utils::MS::window::rect_t moved{window.window_rect};
	/////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////	//move the window up or down depending on the pressed button
	/////////////////////////////////////////////////////////////////////	switch (id)
	/////////////////////////////////////////////////////////////////////		{
	/////////////////////////////////////////////////////////////////////		case utils::input::mouse::button_id::backward: moved.y() -= 10; break;
	/////////////////////////////////////////////////////////////////////		case utils::input::mouse::button_id::forward : moved.y() += 10; break;
	/////////////////////////////////////////////////////////////////////		}
	/////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////	window.window_rect = moved;
	/////////////////////////////////////////////////////////////////////	});
		
	window.show();
	while (window.is_open())
		{
		window.wait_event();
		manager_input.step();
		}
	}

void example::input_system()
	{
	try { body(); }
	catch (const std::system_error& e) { ::MessageBoxA(nullptr, e.what(), "Unhandled Exception", MB_OK | MB_ICONERROR); }
	}