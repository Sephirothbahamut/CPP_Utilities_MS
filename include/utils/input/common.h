#pragma once

#include <array>
#include <cstdint>
#include <functional>

#include <utils/memory.h>
#include <utils/containers/object_pool.h>
#include <utils/math/vec2.h>
#include <utils/math/vec3.h>

namespace utils::input
	{
	template <typename state_T, typename inner_T>
	class input_1d_delta;
	template <typename inner_T>
	class input_2d;

	namespace details
		{
		template <typename state_T>
		class input_base
			{
			public:
				using state_t = state_T;
				using callback            = std::function<void(const state_t& new_state, const state_t& previous_state)>;
				using callbacks_container = utils::containers::object_pool<callback>;
				using callback_handle     = callbacks_container::handle_raw;

				callbacks_container on_changed;
				void changed(const state_t& new_state, const state_t& previous_state) noexcept
					{
					for (auto& action : on_changed)
						{
						action(new_state, previous_state);
						}
					}

			protected:
				void changed(const callback_handle& exclude, const state_t& new_state, const state_t& previous_state) noexcept
					{
					for (auto& action : on_changed)
						{
						if (&action == exclude.get()) { continue; }
						action(new_state, previous_state);
						}
					}
			};
		}

	template <typename state_T>
	class input_1d : public details::input_base<state_T>
		{
		template <typename state_T, typename inner_T>
		friend class input_1d_delta;
		template <typename inner_T>
		friend struct input_2d;

		public:
			using state_t = state_T;
			using callback            = std::function<void(const state_t& new_state, const state_t& previous_state)>;
			using callbacks_container = utils::containers::object_pool<callback>;
			using callback_handle     = callbacks_container::handle_raw;

			const state_t& get_state() const noexcept { return _state; }

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

			virtual void change_state(const callback_handle& exclude, const state_t& new_state) noexcept
				{
				details::input_base<state_T>::changed(exclude, new_state, get_state());
				_state = new_state;
				}
		};

	template <typename state_T, typename inner_T>
	class input_1d_delta : public input_1d<state_T>
		{
		public:
			using state_t = state_T;

			input_1d<inner_T>& positive;
			input_1d<inner_T>& negative;

			input_1d_delta(input_1d<inner_T>& positive, input_1d<inner_T>& negative) : 
				positive{positive}, 
				negative{negative},
				positive_handle
					{
					positive.on_changed.emplace([this](const inner_T& new_positive_state, const inner_T& previous_positive_state)
						{
						this->changed(new_positive_state - negative.get_state(), previous_positive_state - negative.get_state());
						})
					},
				negative_handle
					{
					negative.on_changed.emplace([this](const inner_T& new_negative_state, const inner_T& previous_negative_state)
						{
						this->changed(positive.get_state() - new_negative_state, positive.get_state() - previous_negative_state);
						})
					}
				{}

			const state_t& get_state() const noexcept { return {positive.get_state() - negative.get_state()}; }

		private:
			input_1d<inner_T>::callback_handle positive_handle;
			input_1d<inner_T>::callback_handle negative_handle;
		};

	template <typename inner_T>
	class input_2d : public details::input_base<utils::math::vec2<inner_T>>
		{
		public:
			using state_t = utils::math::vec2<inner_T>;
			using callback            = std::function<void(const state_t& new_state, const state_t& previous_state)>;
			using callbacks_container = utils::containers::object_pool<callback>;
			using callback_handle     = callbacks_container::handle_raw;

			input_1d<inner_T>& x;
			input_1d<inner_T>& y;

			input_2d(input_1d<inner_T>& x, input_1d<inner_T>& y) : 
				x{x}, 
				y{y},
				x_handle
					{
					x.on_changed.emplace([this](const inner_T& new_x_state, const inner_T& previous_x_state)
						{
						this->changed({new_x_state, this->y.get_state()}, {previous_x_state, this->y.get_state()});
						})
					},
				y_handle
					{
					y.on_changed.emplace([this](const inner_T& new_y_state, const inner_T& previous_y_state)
						{
						this->changed({this->x.get_state(), new_y_state}, {this->x.get_state(), previous_y_state});
						})
					}
				{}

			const state_t& get_state() const noexcept { return {x.get_state(), y.get_state()}; }

			/// <summary> Does not trigger callbacks </summary>
			void set_state(const state_t& new_state) noexcept
				{
				x.set_state(new_state.x);
				y.set_state(new_state.y);
				}

			/// <summary> Triggers callbacks </summary>
			void change_state(const state_t& new_state) noexcept
				{
				x.change_state(x_handle, new_state.x);
				y.change_state(y_handle, new_state.y);
				}

		private:
			input_1d<inner_T>::callback_handle x_handle;
			input_1d<inner_T>::callback_handle y_handle;
		};
		
	template <typename inner_T>
	class input_3d : details::input_base<utils::math::vec3<inner_T>>
		{
		public:
			using state_t = utils::math::vec3<inner_T>;
			using callback            = std::function<void(const state_t& new_state, const state_t& previous_state)>;
			using callbacks_container = utils::containers::object_pool<callback>;
			using callback_handle     = callbacks_container::handle_raw;

			input_1d<inner_T>& x;
			input_1d<inner_T>& y;
			input_1d<inner_T>& z;

			input_3d(input_1d<inner_T>& x, input_1d<inner_T>& y, input_1d<inner_T>& z) :
				x{x},
				y{y},
				z{z},
				x_handle
					{
					x.on_changed.emplace([this](const inner_T& new_x_state, const inner_T& previous_x_state)
						{
						this->changed({new_x_state, this->y.get_state(), this->z.get_state()}, {previous_x_state, this->y.get_state(), this->z.get_state()});
						})
					},
				y_handle
					{
					y.on_changed.emplace([this](const inner_T& new_y_state, const inner_T& previous_y_state)
						{
						this->changed({this->x.get_state(), new_y_state, this->z.get_state()}, {this->x.get_state(), previous_y_state, this->z.get_state()});
						})
					},
				z_handle
					{
					z.on_changed.emplace([this](const inner_T& new_z_state, const inner_T& previous_z_state)
						{
						this->changed({this->x.get_state(), this->y.get_state(), new_z_state}, {this->x.get_state(), this->y.get_state(), previous_z_state});
						})
					}
				{}

			const state_t& get_state() const noexcept { return {x.get_state(), y.get_state()}; }

			/// <summary> Does not trigger callbacks </summary>
			void set_state(const state_t& new_state) noexcept
				{
				x.set_state(new_state.x);
				y.set_state(new_state.y);
				z.set_state(new_state.z);
				}

			/// <summary> Triggers callbacks </summary>
			void change_state(const state_t& new_state) noexcept
				{
				x.change_state(x_handle, new_state.x);
				y.change_state(y_handle, new_state.y);
				z.change_state(z_handle, new_state.z);
				}

		private:
			input_1d<inner_T>::callback_handle x_handle;
			input_1d<inner_T>::callback_handle y_handle;
			input_1d<inner_T>::callback_handle z_handle;
		};

	using digital       = input_1d<bool >;
	using digital_delta = input_1d_delta<float, bool>;
	using analog        = input_1d<float>;
	using analog_delta  = input_1d_delta<float, float>;
	using axis2d        = input_2d<float>;
	using axis3d        = input_3d<float>;
	}