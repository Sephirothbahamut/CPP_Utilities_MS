#pragma once

#include "common.h"

namespace utils::input
	{
	struct controller
		{
		enum class button_id 
			{
			dpad_right      ,
			dpad_up         ,
			dpad_left       ,
			dpad_down       ,
			a               ,
			b               ,
			x               ,
			y               ,
			shoulder_r1     ,
			shoulder_l1     ,
			shoulder_r2     ,
			shoulder_l2     ,
			thumbstick_right,
			thumbstick_left ,
			start           ,
			select          
			};

		using buttons_t = inputs<button_id, digital>;
		buttons_t buttons;

		enum class axis_id 
			{
			thumbstick_right_x,
			thumbstick_right_y,
			thumbstick_left_x ,
			thumbstick_left_y 
			};

		using axes_t = inputs<axis_id, analog>;
		axes_t axes;
		
		axis2d thumbstick_right{axes[axis_id::thumbstick_right_x], axes[axis_id::thumbstick_right_y]};
		axis2d thumbstick_left {axes[axis_id::thumbstick_left_x ], axes[axis_id::thumbstick_left_y ]};
 
		digital_delta dpad_x{buttons[button_id::dpad_right], buttons[button_id::dpad_left]};
		digital_delta dpad_y{buttons[button_id::dpad_down ], buttons[button_id::dpad_up  ]};

		axis2d dpad{dpad_x, dpad_y};
		};
	}