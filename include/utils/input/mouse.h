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

		struct
			{
			analog left    ;
			analog right   ;
			analog middle  ;
			analog backward;
			analog forward ;
			
			analog& operator[](button_id button_id)
				{
				switch (button_id)
					{
					case utils::input::mouse::button_id::left    : return left    ;
					case utils::input::mouse::button_id::right   : return right   ;
					case utils::input::mouse::button_id::middle  : return middle  ;
					case utils::input::mouse::button_id::backward: return backward;
					case utils::input::mouse::button_id::forward : return forward ;
					}
				}
			} buttons;

		input_1d<long> x;
		input_1d<long> y;
		input_2d<long> position{x, y};

		void move_by(utils::math::vec2l delta)
			{
			position.set_state(position.get_state() + delta);
			}
		};
	}