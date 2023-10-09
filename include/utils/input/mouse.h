#pragma once

#include <iomanip>
#include <iostream>

#include "common.h"

namespace utils::input_system::device
	{
	namespace details
		{
		enum class button_id
			{
			left,
			right,
			middle,
			backward,
			forward
			};

		enum class axis_id { x, y };
		
		using buttons_t = device::inputs::static_enum<input::digital, button_id>;
		using axes_t    = device::inputs::static_enum<input::analog , axis_id  >;
		}

	struct mouse : device::base<details::buttons_t, details::axes_t>
		{
		using button_id = details::button_id;
		using axis_id   = details::axis_id  ;

		using buttons_t = details::buttons_t;
		using axes_t    = details::axes_t   ;

		class debug_callbacks
			{
			public:
				struct callback_digital_t
					{
					callback_digital_t(button_id id) : id{id} {}
					button_id id;
					utils::input_system::on_completion operator()(const utils::input_system::state_base<bool>& state) const noexcept
						{
						std::cout << utils::enums::enum_name<button_id>(id) << " ";
						if (state.changed()) { std::cout << "(changed)"; }
						else { std::cout << "(unchanged)"; }
						std::cout << " " << state.previous << " > " << state.current << std::endl;
						return utils::input_system::on_completion::remove;
						}
					};

				debug_callbacks(mouse& instance) :
					callbacks_digital
						{
						utils::input_system::event<bool>{callback_digital_t{button_id::left    }},
						utils::input_system::event<bool>{callback_digital_t{button_id::right   }},
						utils::input_system::event<bool>{callback_digital_t{button_id::middle  }},
						utils::input_system::event<bool>{callback_digital_t{button_id::backward}},
						utils::input_system::event<bool>{callback_digital_t{button_id::forward }}
						},
					callback_analog{[](const utils::input_system::state_base<float>& state) -> utils::input_system::on_completion {
						std::cout << "Axis x" << " ";
						if (state.changed()) { std::cout << "(changed)"; }
						else { std::cout << "(unchanged)"; }
						std::cout << " " << state.previous << " > " << state.current << std::endl;
						return utils::input_system::on_completion::remove;
						}}
					{
					callbacks_digital[static_cast<size_t>(button_id::left    )].emplace_mapping<utils::input_system::mapping::button::from_digital>(instance.digital[button_id::left    ]);
					callbacks_digital[static_cast<size_t>(button_id::right   )].emplace_mapping<utils::input_system::mapping::button::from_digital>(instance.digital[button_id::right   ]);
					callbacks_digital[static_cast<size_t>(button_id::middle  )].emplace_mapping<utils::input_system::mapping::button::from_digital>(instance.digital[button_id::middle  ]);
					callbacks_digital[static_cast<size_t>(button_id::backward)].emplace_mapping<utils::input_system::mapping::button::from_digital>(instance.digital[button_id::backward]);
					callbacks_digital[static_cast<size_t>(button_id::forward )].emplace_mapping<utils::input_system::mapping::button::from_digital>(instance.digital[button_id::forward ]);
					
					callback_analog.emplace_mapping<utils::input_system::mapping::axis1d::from_analog>(instance.analog[axis_id::x]);
					}

			private:
				std::array<utils::input_system::event<bool>, buttons_t::count> callbacks_digital;
				utils::input_system::event<float> callback_analog;
			};

		//class debug_callbacks
		//	{
		//	public:
		//		struct callback_digital_t : event::base
		//			{
		//			callback_digital_t(button_id id, input::digital& input) : id{id}, mapping{input} { mapping.map(*this); }
		//
		//			button_id id;
		//			mapping::button::from_digital mapping;
		//
		//			virtual on_completion operator()() noexcept final override
		//				{
		//				std::cout << utils::enums::enum_name<button_id>(id) << " ";
		//				if (mapping.value().changed()) { std::cout << "(changed)"; }
		//				else { std::cout << "(unchanged)"; }
		//				std::cout << " " << mapping.value().previous << " > " << mapping.value().current << std::endl;
		//				return on_completion::remove;
		//				}
		//			};
		//		struct callback_analog_t  : event::base
		//			{
		//			callback_analog_t (std::unique_ptr<mapping::axis1d::base>&& x, std::unique_ptr<mapping::axis1d::base>&& y) : 
		//				mapping{std::move(x), std::move(y)} 
		//				{
		//				mapping.map(*this); 
		//				}
		//
		//			mapping::axis2d::from_axes mapping;
		//
		//			virtual on_completion operator()() noexcept final override
		//				{
		//				std::cout << "XY : (" << mapping.value().previous.x << ", " << mapping.value().previous.y << ") > ("
		//					<< mapping.value().current.x << ", " << mapping.value().current.y << ")" << std::endl;
		//				return on_completion::remove;
		//				}
		//			};
		//
		//		debug_callbacks(mouse& instance) : 
		//			callbacks_digital
		//				{
		//				callback_digital_t{button_id::left    , instance.digital[button_id::left    ]},
		//				callback_digital_t{button_id::right   , instance.digital[button_id::right   ]},
		//				callback_digital_t{button_id::middle  , instance.digital[button_id::middle  ]},
		//				callback_digital_t{button_id::backward, instance.digital[button_id::backward]},
		//				callback_digital_t{button_id::forward , instance.digital[button_id::forward ]}
		//				},
		//			callback_analog
		//				{
		//				std::make_unique<mapping::axis1d::from_analog>(instance.analog[axis_id::x]),
		//				std::make_unique<mapping::axis1d::from_analog>(instance.analog[axis_id::y])
		//				}
		//			{
		//			}
		//
		//	private:
		//		std::array<callback_digital_t, buttons_t::count> callbacks_digital;
		//		callback_analog_t callback_analog;
		//	};
		};
	using maus = mouse;
	///////////////////////////////////////
	using 🐭  = mouse;
	}