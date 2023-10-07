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

namespace utils::input_system
	{
	enum class on_completion { remove, keep };

	template <typename T>
	struct state_base
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

		using ptrs_set = std::unordered_set<utils::observer_ptr<event::base>>;
		}

	class manager
		{
		public:
			void step() noexcept
				{
				auto it{events_ptrs_set.begin()};
				while (it != events_ptrs_set.end())
					{
					auto current{it++};

					auto& triggered_event_ptr{*it};
					auto& triggered_event{*triggered_event_ptr};
					if (triggered_event() == on_completion::remove)
						{
						it = events_ptrs_set.erase(it);
						}
					else
						{
						it++;
						}
					}
				}

			void insert(event::base& event) noexcept { events_ptrs_set.insert(&event); }
			void insert(event::ptrs_set::iterator begin, event::ptrs_set::iterator end) noexcept
				{
				events_ptrs_set.insert(begin, end);
				}

		private:
			event::ptrs_set events_ptrs_set;
		};

	namespace mapping
		{
		template <typename T>
		class base : public ::utils::oop::non_copyable, public ::utils::oop::non_movable
			{
			public:
				using state_type = state_base<T>;
				using value_type = typename state_base<T>::value_type;
				virtual state_type value() const noexcept = 0;
				virtual void map(event::base& event) noexcept { event_ptr = &event; inner_map(event); };
				virtual void unmap() noexcept { if (event_ptr) { inner_unmap(*event_ptr); event_ptr = nullptr; } };

				~base() { unmap(); }

			private:
				::utils::observer_ptr<event::base> event_ptr{nullptr};

				virtual void inner_map  (event::base& event) noexcept = 0;
				virtual void inner_unmap(event::base& event) noexcept = 0;
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
				using state_type = state_base<T>;
				using value_type = typename state_base<T>::value_type;
				const state_type& value() const noexcept { return _value; }
				      state_type& value()       noexcept { return _value; }

				event::ptrs_set events_ptrs_set;
				void change(manager& manager, const value_type& new_value) noexcept
					{
					_value.change(new_value);
					manager.insert(events_ptrs_set.begin(), events_ptrs_set.end());
					}

				void map  (event::base& event) noexcept { events_ptrs_set.insert(&event); }
				void unmap(event::base& event) noexcept { events_ptrs_set.erase (&event); }

			private:
				state_type _value;
			};
		using analog  = base<float>;
		using digital = base<bool >;

		namespace concepts
			{
			template <typename T>
			concept input = std::derived_from<T, base<typename T::value_type>>;
			}
		}

	namespace mapping
		{
		namespace button
			{
			struct base : mapping::base<bool> {};

			class from_digital : public base
				{
				public:
					std::reference_wrapper<input::digital> input_digital;

					from_digital(input::digital& input_digital) : input_digital{input_digital} {}

					virtual state_type value() const noexcept final override
						{
						return input_digital.get().value();
						};

				private:
					virtual void inner_map  (event::base& event) noexcept final override { input_digital.get().map  (event); }
					virtual void inner_unmap(event::base& event) noexcept final override { input_digital.get().unmap(event); }
				};
			class from_analog : public base
				{
				public:
					float threshold{.5f};
					std::reference_wrapper<input::analog> input_analog;

					from_analog(input::analog& input_analog) : input_analog{input_analog} {}

					virtual state_type value() const noexcept final override
						{
						return 
							{
							input_analog.get().value().current  > threshold,
							input_analog.get().value().previous > threshold
							};
						}

				private:
					virtual void inner_map  (event::base& event) noexcept final override { input_analog.get().map  (event); }
					virtual void inner_unmap(event::base& event) noexcept final override { input_analog.get().unmap(event); }
				};
			}

		namespace axis1d
			{
			struct base : mapping::base<float> {};

			class from_digital_b_minus_a : public base
				{
				public:
					std::reference_wrapper<input::digital> input_digital_a;
					std::reference_wrapper<input::digital> input_digital_b;

					from_digital_b_minus_a(input::digital& input_digital_a, input::digital& input_digital_b) :
						input_digital_a{input_digital_a}, input_digital_b{input_digital_b} {}
				
					virtual state_type value() const noexcept final override
						{
						return
							{
							static_cast<float>(input_digital_b.get().value().current  - input_digital_a.get().value().current ),
							static_cast<float>(input_digital_b.get().value().previous - input_digital_a.get().value().previous),
							};
						};

				private:
					virtual void inner_map  (event::base& event) noexcept final override { input_digital_a.get().map  (event); input_digital_b.get().map  (event); }
					virtual void inner_unmap(event::base& event) noexcept final override { input_digital_a.get().unmap(event); input_digital_b.get().unmap(event); }
				};

			class from_analog : public base
				{
				public:
					std::reference_wrapper<input::analog> input_analog;
				
					from_analog(input::analog& input_analog) : input_analog{input_analog} {}

					virtual state_type value() const noexcept final override
						{
						return input_analog.get().value();
						};

				private:
					virtual void inner_map  (event::base& event) noexcept final override { input_analog.get().map  (event); }
					virtual void inner_unmap(event::base& event) noexcept final override { input_analog.get().unmap(event); }
				};
			}

		namespace axis2d
			{
			struct base : mapping::base<::utils::math::vec2f> {};

			class from_axes : public base
				{
				public:
					std::unique_ptr<axis1d::base> mapping_x_ptr{nullptr};
					std::unique_ptr<axis1d::base> mapping_y_ptr{nullptr};

					from_axes(std::unique_ptr<axis1d::base>&& mapping_x, std::unique_ptr<axis1d::base>&& mapping_y) :
						mapping_x_ptr{std::move(mapping_x)}, mapping_y_ptr{std::move(mapping_y)} {}

					virtual state_type value() const noexcept final override
						{
						return
							{
							::utils::math::vec2f{static_cast<float>(mapping_x_ptr->value().current , mapping_y_ptr->value().current)},
							::utils::math::vec2f{static_cast<float>(mapping_x_ptr->value().previous, mapping_y_ptr->value().previous)},
							};
						};

				private:
					virtual void inner_map  (event::base& event) noexcept final override { mapping_x_ptr->map  (event); mapping_y_ptr->map  (event); }
					virtual void inner_unmap(event::base& event) noexcept final override { mapping_x_ptr->unmap(     ); mapping_y_ptr->unmap(     ); }
				};
			}
		}

	namespace device
		{

		enum class test_enum_t { a, b, c };
		namespace inputs
			{
			template <typename DERIVED_T>
			struct base_crtp
				{
				using derived_t = DERIVED_T;
				using id_t      = typename derived_t::id_t;
				using input_t   = typename derived_t::input_t;

				constexpr const derived_t& derived() const noexcept { return static_cast<const derived_t&>(*this); }
				constexpr       derived_t& derived()       noexcept { return static_cast<      derived_t&>(*this); }

				const input_t& operator[](const id_t& id) const noexcept { return derived()[id]; }
				      input_t& operator[](const id_t& id)       noexcept { return derived()[id]; }

				void change(manager& manager, const id_t& id, const typename input_t::value_type& new_value) noexcept
					{
					operator[](id).change(manager, new_value);
					}
				};

			template <input::concepts::input input_T, size_t SIZE>
			struct static_array : public base_crtp<static_array<input_T, SIZE>>
				{
				public:
					using id_t    = size_t;
					using input_t = input_T;
					
					inline static constexpr const size_t count{SIZE};
		
						  input_t& operator[](id_t id) utils_if_release(noexcept)
						{
						if constexpr (utils::compilation::debug) { return at(id); }
						else { return array[id]; }
						}
					const input_t& operator[](id_t id) const utils_if_release(noexcept)
						{
						if constexpr (utils::compilation::debug) { return at(id); }
						else { return array[id]; }
						}
						  input_t& at(id_t id)       { return array.at(static_cast<size_t>(id)); }
					const input_t& at(id_t id) const { return array.at(static_cast<size_t>(id)); }

				private:
					std::array<input_t, SIZE> array;
				};

			template <input::concepts::input input_T, typename id_enum>
			class static_enum : public base_crtp<static_enum<input_T, id_enum>>
				{
				public:
					using id_t    = id_enum;
					using input_t = input_T;
					inline static constexpr const size_t count{utils::enums::enum_count<id_enum>()};
		
						  input_t& operator[](id_t id) utils_if_release(noexcept)
						{
						if constexpr (utils::compilation::debug) { return at(id); }
						else { return array[static_cast<size_t>(id)]; }
						}
					const input_t& operator[](id_t id) const utils_if_release(noexcept)
						{
						if constexpr (utils::compilation::debug) { return at(id); }
						else { return array[static_cast<size_t>(id)]; }
						}
						  input_t& at(id_t id)       { return array.at(static_cast<size_t>(id)); }
					const input_t& at(id_t id) const { return array.at(static_cast<size_t>(id)); }
		
				private:
					std::array<input_t, count> array;
				};

			template <input::concepts::input input_T, typename id_T>
			class dynamic : public base_crtp<dynamic<input_T, id_T>>
				{
				public:
					using id_t    = id_T;
					using input_t = input_T;

						  input_t& operator[](id_t id)       noexcept { return container[id]; }
					const input_t& operator[](id_t id) const noexcept { return container[id]; }

				private:
					std::unordered_map<id_t, input_t> container;
				};

			namespace concepts
				{
				template <typename T>
				concept inputs = std::derived_from<T, base_crtp<typename T::derived_t>>;
				template <typename T>
				concept digital = inputs<T> && std::same_as<typename T::input_t, input::digital>;
				template <typename T>
				concept analog  = inputs<T> && std::same_as<typename T::input_t, input::analog >;
				}
			}

		template <inputs::concepts::digital digital_T, inputs::concepts::analog analog_T>
		struct base
			{
			using digital_t = digital_T;
			using analog_t  = analog_T ;
			inline static constexpr bool different_id_t{!std::same_as<typename digital_t::id_t, typename analog_t::id_t>};

			uintptr_t id;

			digital_t digital;
			analog_t  analog ;

			void change(manager& manager, const typename digital_t::id_t& digital_id, const typename digital_t::value_type& new_value) noexcept
				requires different_id_t
				{
				change_digital(manager, digital_id, new_value);
				}
			void change(manager& manager, const typename analog_t ::id_t& analog_id , const typename analog_t ::value_type& new_value) noexcept
				requires different_id_t
				{
				change_analog (manager, analog_id, new_value);
				}

			void change_digital(manager& manager, const typename digital_t::id_t& id, const typename digital_t::value_type& new_value) noexcept
				{
				digital.change(manager, id, new_value);
				manager.insert(events_ptrs_set.begin(), events_ptrs_set.end());
				}
			void change_analog (manager& manager, const typename analog_t ::id_t& id , const typename analog_t ::value_type& new_value) noexcept
				{
				analog .change(manager, id, new_value);
				manager.insert(events_ptrs_set.begin(), events_ptrs_set.end());
				}

			event::ptrs_set events_ptrs_set;
			};
		}
	}
