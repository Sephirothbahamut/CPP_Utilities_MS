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

	namespace details
		{
		template <typename T>
		struct reference_wrapper_utils
			{
			struct address_hash
				{
				size_t operator()(const std::reference_wrapper<T>& rw) const { return std::hash<T*>{}(&rw.get()); }
				};
			struct address_equality
				{
				size_t operator()(const std::reference_wrapper<T>& a, const std::reference_wrapper<T>& b) const
					{
					return (&a.get()) == (&b.get());
					}
				};
			};

		struct callback_wrapper
			{
			void* identifier;
			bool operator== (const callback_wrapper& other) const noexcept { return identifier == other.identifier; }
			auto operator<=>(const callback_wrapper& other) const noexcept { return identifier <=> other.identifier; }
			on_completion operator()() const noexcept { return callback(); }
			std::function<on_completion()> callback;

			struct hash_function
				{
				size_t operator()(const callback_wrapper& cw) const { return std::hash<void*>{}(cw.identifier); }
				};
			};
		using callbacks_wrappers = std::unordered_set<callback_wrapper, callback_wrapper::hash_function>;

		struct node
			{
			virtual callback_wrapper get_callback_wrapped() noexcept = 0;
			};

		struct branch : node, ::utils::oop::non_copyable, ::utils::oop::non_movable
			{
			branch(node& parent) : parent{parent} {}

			virtual details::callback_wrapper get_callback_wrapped() noexcept final override 
				{
				return parent.get().get_callback_wrapped(); 
				//return parent_obs->get_callback_wrapped();
				}

			//should be referrence wrapper but can't cause child may need to be instantiated before parent
			std::reference_wrapper<node> parent;
			//still assumed to be not null after setup
			//utils::observer_ptr<node> parent_obs;
			};

		using nodes = std::unordered_set
			<
			std::reference_wrapper<branch>,
			reference_wrapper_utils<branch>::address_hash,
			reference_wrapper_utils<branch>::address_equality
			>;
		}

	class manager
		{
		public:
			void step() noexcept
				{
				auto it{callbacks.begin()};
				while (it != callbacks.end())
					{
					const auto& callback{*it};
					if (callback() == on_completion::remove)
						{
						it = callbacks.erase(it);
						}
					else { it++; }
					}
				}

			void insert(const details::callback_wrapper& callback_wrapper) noexcept { callbacks.insert(callback_wrapper); }

		private:
			details::callbacks_wrappers callbacks;
		};

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

				details::nodes nodes;
				void change(manager& manager, const value_type& new_value) noexcept
					{
					_value.change(new_value);
					for (auto node : nodes) { manager.insert(node.get().get_callback_wrapped()); }
					}

				void map  (details::nodes& node) noexcept { nodes.insert(node); }
				void unmap(details::nodes& node) noexcept { nodes.erase (node); }

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

		template <typename T>
		struct root : details::branch
			{
			using branch::branch;
			using state_type = state_base<T>;
			using value_type = typename state_base<T>::value_type;
			virtual state_type value() const noexcept = 0;
			};

		namespace button
			{
			using base = root<bool>;

			struct from_digital : base
				{
				std::reference_wrapper<input::digital> input_digital;

				from_digital(details::node& parent, input::digital& input_digital) : base{parent}, input_digital{input_digital} 
					{
					input_digital.nodes.insert(*this);
					}
				~from_digital() { input_digital.get().nodes.erase(*this); }

				virtual state_type value() const noexcept final override
					{
					return input_digital.get().value();
					};
				};
			}
		namespace axis1d
			{
			using base = root<float>;

			struct from_analog : base
				{
				std::reference_wrapper<input::analog> input_analog;

				from_analog(details::node& parent, input::analog& input_analog) : base{parent}, input_analog{input_analog}
					{
					input_analog.nodes.insert(*this);
					}
				~from_analog() { input_analog.get().nodes.erase(*this); }

				virtual state_type value() const noexcept final override
					{
					return input_analog.get().value();
					};
				};
			}

		namespace button
			{
			struct from_axis : base
				{
				float threshold{.5f};
				std::unique_ptr<axis1d::base> axis_own;

				from_axis(details::node& parent) : base{parent} {}

				template <std::derived_from<axis1d::base> T, typename ...Args>
				auto& emplace_axis(Args&& ...args) noexcept
					{
					axis_own = std::make_unique<T>(*this, std::forward<Args>(args)...);
					return *axis_own;
					}

				virtual state_type value() const noexcept final override
					{
					return 
						{
						axis_own->value().current  > threshold,
						axis_own->value().previous > threshold
						};
					}
				};
			};

		//namespace axis1d
		//	{
		//	struct from_button_b_minus_a : base
		//		{
		//		public:
		//			std::unique_ptr<button::base> button_a_own;
		//			std::unique_ptr<button::base> button_b_own;
		//
		//			from_button_b_minus_a(std::unique_ptr<button::base>&& button_a_own, std::unique_ptr<button::base>&& button_b_own) :
		//				button_a_own{std::move(button_a_own)}, button_b_own{std::move(button_b_own)} {}
		//		
		//			virtual state_type value() const noexcept final override
		//				{
		//				return
		//					{
		//					static_cast<float>(input_digital_b.get().value().current  - input_digital_a.get().value().current ),
		//					static_cast<float>(input_digital_b.get().value().previous - input_digital_a.get().value().previous),
		//					};
		//				};
		//
		//		private:
		//			virtual void inner_map  (event::base& event) noexcept final override { input_digital_a.get().map  (event); input_digital_b.get().map  (event); }
		//			virtual void inner_unmap(event::base& event) noexcept final override { input_digital_a.get().unmap(event); input_digital_b.get().unmap(event); }
		//		};
		//
		//	}
		//
		//namespace axis2d
		//	{
		//	struct base : mapping::base<::utils::math::vec2f> {};
		//
		//	class from_axes : public base
		//		{
		//		public:
		//			std::unique_ptr<axis1d::base> mapping_x_ptr{nullptr};
		//			std::unique_ptr<axis1d::base> mapping_y_ptr{nullptr};
		//
		//			from_axes(std::unique_ptr<axis1d::base>&& mapping_x, std::unique_ptr<axis1d::base>&& mapping_y) :
		//				mapping_x_ptr{std::move(mapping_x)}, mapping_y_ptr{std::move(mapping_y)} {}
		//
		//			virtual state_type value() const noexcept final override
		//				{
		//				return
		//					{
		//					::utils::math::vec2f{mapping_x_ptr->value().current , mapping_y_ptr->value().current },
		//					::utils::math::vec2f{mapping_x_ptr->value().previous, mapping_y_ptr->value().previous},
		//					};
		//				};
		//
		//		private:
		//			virtual void inner_map  (event::base& event) noexcept final override { mapping_x_ptr->map  (event); mapping_y_ptr->map  (event); }
		//			virtual void inner_unmap(event::base& event) noexcept final override { mapping_x_ptr->unmap(     ); mapping_y_ptr->unmap(     ); }
		//		};
		//	}
		}
		
	template <typename T>
	class event : public details::node, public ::utils::oop::non_copyable, public ::utils::oop::non_movable
		{
		public:
			using state_type = state_base<T>;
			using value_type = typename state_base<T>::value_type;

			event() = default;
			event(std::function<on_completion(const state_type& state)> callback) : callback{callback} {}

			using state_type = state_base<T>;
			using value_type = typename state_base<T>::value_type;

			std::unique_ptr<mapping::root<T>> mapping;
			std::function<on_completion(const state_type& state)> callback;

			template <std::derived_from<mapping::root<T>> T, typename ...Args>
			auto& emplace_mapping(Args&& ...args) noexcept
				{
				mapping = std::make_unique<T>(*this, std::forward<Args>(args)...);
				return *mapping;
				}

		private:
			state_type state;
			virtual details::callback_wrapper get_callback_wrapped() noexcept final override
				{
				return {reinterpret_cast<void*>(this), [this]()
					{
					state = mapping->value();
					auto result{callback(state)};
					if (result == on_completion::keep) { state.change(state.current); }
					return result;
					}};
				};
		};


	namespace device
		{
		namespace inputs
			{
			template <typename DERIVED_T, input::concepts::input input_T, typename id_T>
			struct base_crtp
				{
				using derived_t = DERIVED_T;
				using id_t      = id_T;
				using input_t   = input_T;

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
			class static_array : public base_crtp<static_array<input_T, SIZE>, input_T, size_t>
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
			class static_enum : public base_crtp<static_enum<input_T, id_enum>, input_T, id_enum>
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
			class dynamic : public base_crtp<dynamic<input_T, id_T>, input_T, id_T>
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
				concept inputs = std::derived_from<T, base_crtp<typename T::derived_t, typename T::input_t, typename T::id_t>>;
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

			void change(manager& manager, const typename digital_t::id_t& digital_id, const typename digital_t::input_t::value_type& new_value) noexcept
				requires different_id_t
				{
				change_digital(manager, digital_id, new_value);
				}
			void change(manager& manager, const typename analog_t ::id_t& analog_id , const typename analog_t ::input_t::value_type& new_value) noexcept
				requires different_id_t
				{
				change_analog (manager, analog_id, new_value);
				}

			void change_digital(manager& manager, const typename digital_t::id_t& id, const typename digital_t::input_t::value_type& new_value) noexcept
				{
				digital.change(manager, id, new_value);
				}
			void change_analog (manager& manager, const typename analog_t ::id_t& id , const typename analog_t ::input_t::value_type& new_value) noexcept
				{
				analog .change(manager, id, new_value);
				}
			};
		}
	}
