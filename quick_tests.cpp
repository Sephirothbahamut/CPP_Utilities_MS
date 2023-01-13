#include <utils/enum.h>
#include <iostream>
#include <chrono>

#include "include/utils/MS/window/window.h"
#include "include/utils/MS/window/style.h"
#include "include/utils/MS/window/taskbar.h"
#include "include/utils/MS/window/regions.h"
#include "include/utils/MS/window/snap_on_drag.h"
#include "include/utils/MS/window/input/mouse.h"

//#include "include/utils/graphics/d2d.h"
//#include "include/utils/graphics/d2d/window.h"

// Let it be recorded to history that I wanted to use '🗔' instead of "window" for the window namespace

int main()
	{

	//try
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
				},
			// Add the input::mouse module for my input system
			utils::MS::window::input::mouse::create_info{}
			};

		// Now let's add some modules to the window at any time after window creation.
		// Note: Thanks to my fancy and untested (XD) object_pool container, 
		// you can add modules even while the modules container is being iterated without worries!
		// Adding a module this way returns an handle through which you can access the module itself.

		// You can create a new module with this method if that module has a create_info struct
		auto window_regions_handle{window.emplace_module_from_create_info(utils::MS::window::regions::create_info
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

		// If a module has no create_info struct you can add it this way instead. Acts as any other std container's emplace, however you should still omit the 1st constructor parameter, the reference to window::base.
		auto window_snap_handle{window.emplace_module<utils::MS::window::snap_on_drag>()};
		
		// You can also retrieve an handle to a component that was added on creation or at any other point
		auto window_module_mouse_handle{window.get_module_handle<utils::MS::window::input::mouse>()};

		window_module_mouse_handle->default_mouse.buttons.on_changed.emplace([&window](const utils::input::mouse::button_id& id, const bool& state, const bool&) 
			{
			std::cout << "Mouse button " << utils::enums::enum_name<utils::input::mouse::button_id>(id) << " " << (state ? "pressed" : "released") << std::endl; 
		
			utils::MS::window::rect_t rect;
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
		
		huq handle_left    {window_module_mouse_handle->default_mouse.buttons[utils::input::mouse::button_id::left    ].on_changed.make_unique([&handle_left    ](const bool& state, const bool&)
			{
			std::cout << "Self-removing mouse button left     " << (state ? "pressed" : "released") << std::endl; 
			handle_left    .reset();
			})};
		huq handle_right   {window_module_mouse_handle->default_mouse.buttons[utils::input::mouse::button_id::right   ].on_changed.make_unique([&handle_right   ](const bool state, const bool)
			{
			std::cout << "Self-removing mouse button right    " << (state ? "pressed" : "released") << std::endl;
			handle_right   .reset();
			})};
		huq handle_middle  {window_module_mouse_handle->default_mouse.buttons[utils::input::mouse::button_id::middle  ].on_changed.make_unique([&handle_middle  ](const bool state, const bool)
			{
			std::cout << "Self-removing mouse button middle   " << (state ? "pressed" : "released") << std::endl;
			handle_middle  .reset();
			})};
		huq handle_forward {window_module_mouse_handle->default_mouse.buttons[utils::input::mouse::button_id::forward ].on_changed.make_unique([&handle_forward ](const bool state, const bool)
			{
			std::cout << "Self-removing mouse button forward  " << (state ? "pressed" : "released") << std::endl;
			handle_forward .reset();
			})};
		huq handle_backward{window_module_mouse_handle->default_mouse.buttons[utils::input::mouse::button_id::backward].on_changed.make_unique([&handle_backward](const bool state, const bool)
			{
			std::cout << "Self-removing mouse button backward " << (state ? "pressed" : "released") << std::endl;
			handle_backward.reset();
		
			//Busy loop to force debugger hot path
			auto tp{std::chrono::steady_clock::now()};
			while ((std::chrono::steady_clock::now() - tp) < std::chrono::seconds(10));
			})};
		
		window_module_mouse_handle->default_mouse.position.on_changed.emplace([](const utils::math::vec2l& new_position, const utils::math::vec2l&)
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