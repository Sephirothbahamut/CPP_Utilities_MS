#pragma once

#include <iomanip>
#include <iostream>

#include "common.h"

namespace utils::input
	{
	struct mouse
		{
		enum class button_id 
			{
			left    ,
			right   ,
			middle  ,
			backward,
			forward ,
			};

		using buttons_t = inputs<button_id, digital>;
		buttons_t buttons;

		enum class axis_id { x, y };

		using axes_t = inputs<axis_id, input_1d<long>>;
		axes_t axes;

		input_2d<long> position{axes[axis_id::x], axes[axis_id::y]};

		void move_by(utils::math::vec2l delta)
			{
			position.set_state(position.get_state() + delta);
			}

		input_event leave;


		class debug_callbacks_handles
			{
			friend class mouse;
			public:
				void unregister() noexcept { buttons.reset(); axes.reset(); position.reset(); }

			private:
				using chu_b = typename buttons_t     ::callback_handle_unique;
				using chu_a = typename axes_t        ::callback_handle_unique;
				using chu_2 = typename input_2d<long>::callback_handle_unique;
				using chu_e = typename input_event   ::callback_handle_unique;

				chu_b buttons;
				chu_a axes;
				chu_2 position;
				chu_e leave;

				debug_callbacks_handles(chu_b&& buttons, chu_a&& axes, chu_2&& position, chu_e&& leave) : buttons{std::move(buttons)}, axes{std::move(axes)}, position{std::move(position)}, leave{std::move(leave)} {}
			};
		debug_callbacks_handles register_debug_callbacks()
			{
			return 
				{
				{this->buttons .on_changed.make_unique([](const button_id& id, const bool& state, const bool&) {                                std::cout << "Mouse button  " << std::left << std::setw(9) << utils::enums::enum_name(id) << (state ? "pressed" : "released") << std::endl; })},
				{this->axes    .on_changed.make_unique([](const axis_id  & id, const long& state, const long&) {                                std::cout << "Mouse axis    " << utils::enums::enum_name(id) << ": " << state << std::endl; })},
				{this->position.on_changed.make_unique([](                     const auto& state, const auto&) { using namespace utils::output; std::cout << "Mouse position: " << state << std::endl; })},
				{this->leave   .on_trigger.make_unique([]() { std::cout << "Mouse left" << std::endl; })},
				};
			}
		};
	}