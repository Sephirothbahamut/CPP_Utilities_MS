#pragma once

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
		};
	}