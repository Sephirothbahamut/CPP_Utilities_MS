#pragma once

#include "window.h"

namespace utils::MS::window
	{
	class style : public module
		{
		public:
			enum class transparency_t
				{
				composition_attribute,
				DWM_blurbehind,
				DWM_transparent,
				DWM_margin,
				layered,
				none
				};
			enum class value_t { enable, disable, _default };

			struct create_info
				{
				using module_type = style;
				transparency_t transparency{transparency_t::none};
				value_t borders{value_t::_default};
				value_t shadow {value_t::_default};
				/// <summary> Only used by layered transparency </summary>
				uint8_t alpha{255};

				void adjust_base_create_info(utils::MS::window::base::create_info& base_create_info) const noexcept;
				};

			style(window::base& base, create_info create_info = {});

		private:
			virtual procedure_result procedure(UINT msg, WPARAM wparam, LPARAM lparam) override;

		private:
			value_t borders;
		};
	}

#ifdef utils_implementation
#include "style.cpp"
#endif