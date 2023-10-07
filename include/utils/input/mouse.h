#pragma once

#include <iomanip>
#include <iostream>

#include "common.h"

namespace utils::input_system::device
	{
	namespace mouse
		{
		enum class button_id 
			{
			left    ,
			right   ,
			middle  ,
			backward,
			forward 
			};

		enum class axis_id { x, y };

		using buttons_t = device::inputs::static_enum<input::digital, mouse::button_id>;
		using axes_t    = device::inputs::static_enum<input::analog , mouse::axis_id  >;

		using object = device::base<buttons_t, axes_t>;//decent-er name?

		class debug_callbacks
			{
			public:
				struct callback_digital_t : event::base
					{
					callback_digital_t(button_id id, input::digital& input) : id{id}, mapping{input} { mapping.map(*this); }

					button_id id;
					mapping::button::from_digital mapping;

					virtual on_completion operator()() noexcept final override
						{
						std::cout << utils::enums::enum_name<button_id>(id) << " ";
						if (mapping.value().changed()) { std::cout << "(changed)"; }
						else { std::cout << "(unchanged)"; }
						std::cout << " " << mapping.value().previous << " > " << mapping.value().current << std::endl;
						}
					};

				debug_callbacks(object& instance) : 
					instance{instance},
					callbacks_digital
						{
						callback_digital_t{button_id::left    , instance.digital[button_id::left    ]},
						callback_digital_t{button_id::right   , instance.digital[button_id::right   ]},
						callback_digital_t{button_id::middle  , instance.digital[button_id::middle  ]},
						callback_digital_t{button_id::backward, instance.digital[button_id::backward]},
						callback_digital_t{button_id::forward , instance.digital[button_id::forward ]}
						}
					{
					
					}



			private:
				std::reference_wrapper<object> instance;

				std::array<callback_digital_t, buttons_t::count> callbacks_digital;
			};
		};
	}