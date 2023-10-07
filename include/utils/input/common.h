#pragma once

#include <map>
#include <set>
#include <unordered_set>
#include <array>
#include <vector>
#include <cstdint>
#include <functional>

#include <utils/enum.h>
#include <utils/oop/disable_move_copy.h>
#include <utils/memory.h>
#include <utils/math/vec2.h>
#include <utils/math/vec3.h>
#include <utils/containers/object_pool.h>
#include <utils/polymorphic_value.h>

namespace utils::beta::input_system
	{
	enum class on_completion { remove, keep };

	template <typename T>
	struct value_base
		{
		using value_type = T;
		T current;
		T previous;
		void change(const T& new_value) noexcept { previous = current; current = new_value; }
		bool changed() const noexcept { return current != previous; }
		};

	namespace event
		{
		struct base : ::utils::oop::non_copyable, ::utils::oop::non_movable
			{
			virtual on_completion operator()() = 0;
			};
		}

	namespace mapping
		{
		template <typename T>
		struct base : ::utils::oop::non_copyable, ::utils::oop::non_movable
			{
			using value_type = value_base<T>;
			virtual value_type value() const noexcept = 0;
			virtual void map  (event::base& event) noexcept = 0;
			virtual void unmap(event::base& event) noexcept = 0;
			};


		namespace concepts
			{
			template <typename T>
			concept mapping = std::derived_from<T, base<typename T::value_type>>;
			}
		}

	namespace input
		{
		template <typename T>
		struct base : utils::oop::non_copyable, utils::oop::non_movable
			{
			public:
				using value_type = value_base<T>;
				const value_type& value() const noexcept { return _value; }
				value_type& value()       noexcept { return _value; }

				std::unordered_set<utils::observer_ptr<event::base>> events_ptrs;
				void change(const T& new_value, std::unordered_set<utils::observer_ptr<event::base>>& triggered_events_ptrs) noexcept
					{
					_value.change(new_value);
					triggered_events_ptrs.insert(events_ptrs.begin(), events_ptrs.end());
					}

				void map  (event::base& event) noexcept { events_ptrs.insert(&event); }
				void unmap(event::base& event) noexcept { events_ptrs.erase (&event); }

			private:
				value_type _value;
			};
		using analog  = base<float>;
		using digital = base<bool >;
		}

	namespace mappings
		{
		namespace button
			{
			struct base : mapping::base<bool> {};

			struct from_digital : base
				{
				std::reference_wrapper<input::digital> input_digital;

				from_digital(input::digital& input_digital) : input_digital{input_digital} {}

				virtual void map  (event::base& event) noexcept final override { input_digital.get().map  (event); }
				virtual void unmap(event::base& event) noexcept final override { input_digital.get().unmap(event); }

				virtual value_type value() const noexcept final override
					{
					return input_digital.get().value();
					};
				};
			struct from_analog : base
				{
				float threshold{.5f};
				std::reference_wrapper<input::analog> input_analog;

				from_analog(input::analog& input_analog) : input_analog{input_analog} {}

				virtual void map  (event::base& event) noexcept final override { input_analog.get().map  (event); }
				virtual void unmap(event::base& event) noexcept final override { input_analog.get().unmap(event); }

				virtual value_type value() const noexcept final override
					{
					return 
						{
						input_analog.get().value().current  > threshold,
						input_analog.get().value().previous > threshold
						};
					}
				};
			}

		namespace axis1d
			{
			struct base : mapping::base<float> {};

			struct from_digital_b_minus_a : base
				{
				std::reference_wrapper<input::digital> input_digital_a;
				std::reference_wrapper<input::digital> input_digital_b;

				from_digital_b_minus_a(input::digital& input_digital_a, input::digital& input_digital_b) :
					input_digital_a{input_digital_a}, input_digital_b{input_digital_b} {}
				
				virtual void map  (event::base& event) noexcept final override { input_digital_a.get().map  (event); input_digital_b.get().map  (event); }
				virtual void unmap(event::base& event) noexcept final override { input_digital_a.get().unmap(event); input_digital_b.get().unmap(event); }

				virtual value_type value() const noexcept final override
					{
					return
						{
						static_cast<float>(input_digital_b.get().value().current  - input_digital_a.get().value().current ),
						static_cast<float>(input_digital_b.get().value().previous - input_digital_a.get().value().previous),
						};
					};
				};

			struct from_analog : base
				{
				std::reference_wrapper<input::analog> input_analog;
				
				from_analog(input::analog& input_analog) : input_analog{input_analog} {}

				virtual void map  (event::base& event) noexcept final override { input_analog.get().map  (event); }
				virtual void unmap(event::base& event) noexcept final override { input_analog.get().unmap(event); }

				virtual value_type value() const noexcept final override
					{
					return input_analog.get().value();
					};
				};
			}

		namespace axis2d
			{
			struct base : mapping::base<::utils::math::vec2f> {};

			struct from_axes : base
				{
				std::unique_ptr<axis1d::base> mapping_x_ptr{nullptr};
				std::unique_ptr<axis1d::base> mapping_y_ptr{nullptr};

				from_axes(std::unique_ptr<axis1d::base>&& mapping_x, std::unique_ptr<axis1d::base>&& mapping_y) :
					mapping_x_ptr{std::move(mapping_x)}, mapping_y_ptr{std::move(mapping_y)} {}

				virtual void map(event::base& event) noexcept final override { mapping_x_ptr->map(event); mapping_y_ptr->map(event); }
				virtual void unmap(event::base& event) noexcept final override { mapping_x_ptr->unmap(event); mapping_y_ptr->unmap(event); }

				virtual value_type value() const noexcept final override
					{
					return
						{
						::utils::math::vec2f{static_cast<float>(mapping_x_ptr->value().current , mapping_y_ptr->value().current)},
						::utils::math::vec2f{static_cast<float>(mapping_x_ptr->value().previous, mapping_y_ptr->value().previous)},
						};
					};
				};
			}
		}

	//namespace details
	//	{
	//	namespace concepts
	//		{
	//		template <typename T>
	//		concept value_input = std::same_as<T, value_base<typename T::value_type>>;
	//		}
	//
	//	template <typename id_T, concepts::value_input input_T>
	//	class inputs_base
	//		{
	//		public:
	//			using id_t = id_T;
	//			using input_t = input_T;
	//
	//			void change(this const auto&& self, id_t, const input_t::value_type& new_value) noexcept
	//				{
	//
	//				}
	//
	//		private:
	//		};
	//
	//	template <typename id_enum, concepts::input input_t>
	//	class inputs_enum : public inputs_base<id_enum, input_t>
	//		{
	//		public:
	//			using id_t = id_enum;
	//			inline static constexpr const size_t count{utils::enums::enum_count<id_t>()};
	//
	//			input_t& operator[](id_t id) utils_if_release(noexcept)
	//				{
	//				if constexpr (utils::compilation::debug) { return at(id); }
	//				else { return array[static_cast<size_t>(id)]; }
	//				}
	//			const input_t& operator[](id_t id) const utils_if_release(noexcept)
	//				{
	//				if constexpr (utils::compilation::debug) { return at(id); }
	//				else { return array[static_cast<size_t>(id)]; }
	//				}
	//			input_t& at(id_t id) { return array.at(static_cast<size_t>(id)); }
	//			const input_t& at(id_t id) const { return array.at(static_cast<size_t>(id)); }
	//
	//			void changed(const id_t& id, const state_t& new_state, const state_t& previous_state) noexcept
	//				{
	//				for (auto& action : on_changed)
	//					{
	//					action(id, new_state, previous_state);
	//					}
	//				}
	//
	//		private:
	//			std::array<input_t, count> array;
	//		};
	//	}


	class device_base
		{
		public:
			uintptr_t id;

			auto get_container_bool (this const auto&& self) noexcept { return self.container_digital; }
			auto get_container_float(this const auto&& self) noexcept { return self.container_analog ; }

		private:
		};

	template <typename enum_digital_T, typename enum_analog_T>
	class device_static : public device_base
		{
		public:
			using enum_digital_t = enum_digital_T;
			using enum_analog_t  = enum_analog_T;

		private:
			details::inputs_static<enum_bools_t , bool > container_digital;
			details::inputs_static<enum_analog_t, float> container_analog ;
		};

	class device_dynamic : public device_base
		{
		public:

		private:
			std::unordered_map<size_t, bool > container_digital;
			std::unordered_map<size_t, float> container_analog ;
		};
	}

namespace utils::input
	{
	//namespace tmp //TODO remove
	//	{
	//	inline std::vector<RAWINPUTDEVICELIST> GetRawInputDevices()
	//		{
	//		UINT deviceCount = 10; // initial guess, must be nonzero
	//		std::vector<RAWINPUTDEVICELIST> devices(deviceCount);
	//		while (deviceCount != 0) {
	//			UINT actualDeviceCount = GetRawInputDeviceList(
	//				devices.data(), &deviceCount,
	//				sizeof(devices[0]));
	//			if (actualDeviceCount != (UINT)-1) {
	//				devices.resize(actualDeviceCount);
	//				return devices;
	//				}
	//			DWORD error = GetLastError();
	//			if (error != ERROR_INSUFFICIENT_BUFFER) {
	//				std::terminate(); // throw something
	//				}
	//			devices.resize(deviceCount);
	//			}
	//		}
	//
	//	inline void cout_raw_input_devices()
	//		{
	//		for(const auto& device : GetRawInputDevices())
	//			{
	//			switch (device.dwType)
	//				{
	//				case RIM_TYPEKEYBOARD: std::cout << "Keyboard "; break;
	//				case RIM_TYPEMOUSE   : std::cout << "Mouse    "; break;
	//				case RIM_TYPEHID     : std::cout << "HID      "; break;
	//				default:               std::cout << "Other    "; break;
	//				}
	//			std::cout << "(" << reinterpret_cast<uintptr_t>(device.hDevice) << ")" << std::endl;
	//			}
	//		}
	//	}

	namespace details
		{
		template <typename state_T>
		class input_base
			{
			public:
				using state_t                = state_T;
				using callback_signature     = void(const state_t& new_state, const state_t& previous_state);
				using callback               = std::function<callback_signature>;
				using callbacks_container    = utils::containers::object_pool<callback>;
				using callback_handle_raw    = callbacks_container::handle_raw   ;
				using callback_handle_unique = callbacks_container::handle_unique;
				using callback_handle_shared = callbacks_container::handle_shared;

				callbacks_container on_changed;
				void changed(const state_t& new_state, const state_t& previous_state) noexcept
					{
					for (auto& action : on_changed)
						{
						action(new_state, previous_state);
						}
					}

			protected:
				void changed(const callback_handle_raw& exclude, const state_t& new_state, const state_t& previous_state) noexcept
					{
					for (auto& action : on_changed)
						{
						if (&action == exclude.get()) { continue; }
						action(new_state, previous_state);
						}
					}
			};
		}

	class input_event
		{
		public:
			using state_t                = void;
			using callback_signature     = void();
			using callback               = std::function<callback_signature>;
			using callbacks_container    = utils::containers::object_pool<callback>;
			using callback_handle_raw    = callbacks_container::handle_raw   ;
			using callback_handle_unique = callbacks_container::handle_unique;
			using callback_handle_shared = callbacks_container::handle_shared;

			callbacks_container on_trigger;
			void trigger() noexcept
				{
				for (auto& action : on_trigger)
					{
					action();
					}
				}

		protected:
			void trigger(const callback_handle_raw& exclude) noexcept
				{
				for (auto& action : on_trigger)
					{
					if (&action == exclude.get()) { continue; }
					action();
					}
				}
		};

	template <typename state_T>
	class input_1d : public details::input_base<state_T>
		{
		template <typename state_T, typename inner_T>
		friend class input_1d_delta;
		template <typename inner_T>
		friend class input_2d;

		public:
			using typename details::input_base<state_T>::state_t               ;
			using typename details::input_base<state_T>::callback_signature    ;
			using typename details::input_base<state_T>::callback              ;
			using typename details::input_base<state_T>::callbacks_container   ;
			using typename details::input_base<state_T>::callback_handle_raw   ;
			using typename details::input_base<state_T>::callback_handle_unique;
			using typename details::input_base<state_T>::callback_handle_shared;

			virtual const state_t& get_state() const noexcept { return _state; }

			/// <summary> Does not trigger callbacks </summary>
			virtual void set_state(const state_t& new_state) noexcept
				{
				_state = new_state;
				}

			/// <summary> Triggers callbacks </summary>
			virtual void change_state(const state_t& new_state) noexcept
				{
				details::input_base<state_T>::changed(new_state, get_state());
				_state = new_state;
				}

		protected:
			state_t _state{0};

			virtual void change_state(const callback_handle_raw& exclude, const state_t& new_state) noexcept
				{
				details::input_base<state_T>::changed(exclude, new_state, get_state());
				_state = new_state;
				}
		};

	template <typename state_T, typename inner_T>
	class input_1d_delta : public input_1d<state_T>
		{
		public:
			using typename details::input_base<state_T>::state_t               ;
			using typename details::input_base<state_T>::callback_signature    ;
			using typename details::input_base<state_T>::callback              ;
			using typename details::input_base<state_T>::callbacks_container   ;
			using typename details::input_base<state_T>::callback_handle_raw   ;
			using typename details::input_base<state_T>::callback_handle_unique;
			using typename details::input_base<state_T>::callback_handle_shared;

			std::reference_wrapper<input_1d<inner_T>> positive;
			std::reference_wrapper<input_1d<inner_T>> negative;

			input_1d_delta(input_1d<inner_T>& positive, input_1d<inner_T>& negative) : 
				positive{positive}, 
				negative{negative},
				positive_handle
					{
					positive.on_changed.make_unique([this](const inner_T& new_positive_state, const inner_T& previous_positive_state)
						{
						this->changed(new_positive_state - negative->get_state(), previous_positive_state - negative->get_state());
						})
					},
				negative_handle
					{
					negative.on_changed.make_unique([this](const inner_T& new_negative_state, const inner_T& previous_negative_state)
						{
						this->changed(positive->get_state() - new_negative_state, positive->get_state() - previous_negative_state);
						})
					}
				{}

			const state_t& get_state() const noexcept override { return {positive.get_state() - negative.get_state()}; }

		private:
			input_1d<inner_T>::callback_handle_unique positive_handle;
			input_1d<inner_T>::callback_handle_unique negative_handle;
		};

	using digital       = input_1d<bool >;
	using digital_delta = input_1d_delta<float, bool>;
	using analog        = input_1d<float>;
	using analog_delta  = input_1d_delta<float, float>;
	using axis2d        = input_2d<float>;
	using axis3d        = input_3d<float>;



	template <typename id_enum, typename input_t>
	class inputs
		{
		public:
			using id = id_enum;
			inline static constexpr const size_t count{utils::enums::enum_count<id>()};
			using state_t = typename input_t::state_t;

			using callback_signature     = void(const id& id, const state_t& new_state, const state_t& previous_state);
			using callback               = std::function<callback_signature>;
			using callbacks_container    = utils::containers::object_pool<callback>;
			using callback_handle_raw    = callbacks_container::handle_raw   ;
			using callback_handle_unique = callbacks_container::handle_unique;
			using callback_handle_shared = callbacks_container::handle_shared;

			callbacks_container on_changed;
			
			input_t& operator[](id id) utils_if_release(noexcept) 
					{
					if constexpr (utils::compilation::debug) { return at(id); }
					else { return array[static_cast<size_t>(id)]; }
					}
			const input_t& operator[](id id) const utils_if_release(noexcept) 
					{
					if constexpr (utils::compilation::debug) { return at(id); }
					else { return array[static_cast<size_t>(id)]; }
					}
			      input_t& at(id id)       { return array.at(static_cast<size_t>(id)); }
			const input_t& at(id id) const { return array.at(static_cast<size_t>(id)); }

			void changed(const id& id, const state_t& new_state, const state_t& previous_state) noexcept
				{
				for (auto& action : on_changed)
					{
					action(id, new_state, previous_state);
					}
				}

		private:
			std::array<input_t, count> array;
		};
	
	template <typename input_t>
	class inputs_dynamic
		{
		public:
			using id = size_t;
			using state_t = typename input_t::state_t;

			using callback_signature     = void(const id& id, const state_t& new_state, const state_t& previous_state);
			using callback               = std::function<callback_signature>;
			using callbacks_container    = utils::containers::object_pool<callback>;
			using callback_handle_raw    = callbacks_container::handle_raw   ;
			using callback_handle_unique = callbacks_container::handle_unique;
			using callback_handle_shared = callbacks_container::handle_shared;

			callbacks_container on_changed;
			
			input_t& operator[](id id) utils_if_release(noexcept) 
					{
					if constexpr (utils::compilation::debug) { return at(id); }
					else { return map[id]; }
					}
			const input_t& operator[](id id) const utils_if_release(noexcept) 
					{
					if constexpr (utils::compilation::debug) { return at(id); }
					else { return map[id]; }
					}
			      input_t& at(id id)       { return map.at(id); }
			const input_t& at(id id) const { return map.at(id); }

		private:
			std::unordered_map<id, input_t> array;

			void changed(const id& id, const state_t& new_state, const state_t& previous_state) noexcept
				{
				for (auto& action : on_changed)
					{
					action(id, new_state, previous_state);
					}
				}
		};
	}