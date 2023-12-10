#include <iostream>
#include <chrono>

#include <utils/enum.h>

#include <utils/MS/window/window.h>
#include <utils/MS/window/input/mouse.h>
#include <utils/MS/window/input/keyboard.h>

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
		utils::MS::window::input::mouse   ::create_info{manager_input},
		utils::MS::window::input::keyboard::create_info{manager_input},
		close_module::create_info{}
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
	auto mouse_debug_callbacks{default_mouse.bind_debug_callbacks()};

	// Now let's add something fancier

	// Register an event that unregisters the debug callbacks when right click is pressed
	// Note: the last parameter is the previous state. In general it's useless for digital inputs like mouse buttons,
	// as it's always going to be !state. However it can be useful for analog ones.
	// Since we're not using it at all, I'm leaving it unnamed.

	//utils::input_system::event::mapped<bool> event_pippo {[](const utils::input_system::state<bool>& state)
	//	{
	//	if (state.current && !state.previous)
	//		{
	//		std::cout << "pippo begin" << std::endl;
	//		return utils::input_system::on_completion::keep;
	//		}
	//	if (!state.current && state.previous)
	//		{
	//		std::cout << "pippo end" << std::endl;
	//
	//		//mouse_debug_callbacks_opt = std::nullopt;
	//
	//		return utils::input_system::on_completion::remove;
	//		}
	//	static size_t count{0};
	//	std::cout << "pippo" << count << std::endl;
	//	count++;
	//	return utils::input_system::on_completion::keep;
	//	}};
	//event_pippo.emplace_mapping<utils::input_system::mapping::button::device_input>(default_mouse.digital[utils::input_system::device::mouse::button_id::right]);
	//
	//auto event_raw_pippo{default_mouse.digital.emplace_event([](const auto& state)
	//	{
	//	if (state.current && !state.previous)
	//		{
	//		std::cout << "pluto begin " << utils::enums::enum_name(state.id) << std::endl;
	//		return utils::input_system::on_completion::keep;
	//		}
	//	if (!state.current && state.previous)
	//		{
	//		std::cout << "pluto end " << utils::enums::enum_name(state.id) << std::endl;
	//
	//		//mouse_debug_callbacks_opt = std::nullopt;
	//
	//		return utils::input_system::on_completion::remove;
	//		}
	//
	//	std::cout << "pluto " << utils::enums::enum_name(state.id) << std::endl;
	//	return utils::input_system::on_completion::keep;
	//	})};

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
	//*
	while (window.is_open())
		{
		window.wait_event();
		manager_input.step();
		}
	/*/
	while (window.is_open())
		{
		while (window.poll_event());
		std::cout << "___________________________________" << std::endl;
		manager_input.step();
		std::this_thread::sleep_for(std::chrono::milliseconds(250));
		}
	/**/
	}

void example::input_system()
	{
	try { body(); }
	catch (const std::exception& e) { std::cout << e.what() << std::endl; }
	}