#pragma once

#include "common.h"

namespace utils::input
	{
	struct controller
		{
		// I know this is UB, but I like to live dangerously. Until a bug is reported I'll keep it.
		// If you don't like UB: replace the non-array union members with msvc's properties
		union
			{
			std::array<digital, 16> array{};
			struct
				{
				digital dpad_right;
				digital dpad_up;
				digital dpad_left;
				digital dpad_down;
				digital a;
				digital b;
				digital x;
				digital y;
				digital shoulder_r1;
				digital shoulder_l1;
				digital shoulder_r2;
				digital shoulder_l2;
				digital thumbstick_right;
				digital thumbstick_left;
				digital start;
				digital select;
				};
			} buttons;

		analog thumbstick_right_x;
		analog thumbstick_right_y;

		analog thumbstick_left_x;
		analog thumbstick_left_y;

		axis2d thumbstick_right{thumbstick_right_x, thumbstick_right_y};
		axis2d thumbstick_left {thumbstick_left_x , thumbstick_left_y };

		digital_delta dpad_x{buttons.dpad_right, buttons.dpad_left};
		digital_delta dpad_y{buttons.dpad_down , buttons.dpad_up  };

		axis2d dpad{dpad_x, dpad_y};
		};
	}