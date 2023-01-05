#pragma once

#include "window.h"
#include "details/style.h"

namespace utils::win32::window
	{
	class style : public module
		{
		public:
			enum class transparency_t
				{
				composition_attribute,
				DWM_blurbehind,
				DWM_margin,
				layered,
				none
				};
			enum class value_t { enable, disable, _default };

			struct create_info
				{
				transparency_t transparency{transparency_t::none};
				value_t borders{value_t::_default};
				value_t shadow {value_t::_default};
				/// <summary> Only used by layered transparency </summary>
				uint8_t alpha{127};

				inline utils::win32::window::base::create_info adjust_base_create_info(const utils::win32::window::base::create_info& base_create_info) const noexcept
					{
					utils::win32::window::base::create_info ret{base_create_info};

					if (transparency == transparency_t::layered)
						{
						ret.style_ex |= WS_EX_LAYERED;
						}

					if (borders == value_t::disable)
						{
						DWORD style{static_cast<DWORD>(details::style::Style::windowed)};
						ret.style |= style;
						}

					return ret;
					}
				};

			style(window::base& base, create_info create_info) : 
				module
					{
					base,
					[this](UINT msg, WPARAM wparam, LPARAM lparam) -> std::optional<LRESULT> { return procedure(msg, wparam, lparam); }
					}, 
				borders{create_info.borders}
				{
				switch (create_info.transparency)
					{
					case transparency_t::composition_attribute:
						details::make_glass_CompositionAttribute(base.get_handle());
						break;
					case transparency_t::DWM_blurbehind:
						details::make_glass_DWM_BlurBehind      (base.get_handle());
						break;
					case transparency_t::DWM_margin:
						details::make_glass_DWM_margin          (base.get_handle());
						break;
					case transparency_t::layered:
						details::make_transparent_Layered       (base.get_handle(), create_info.alpha);
						break;

					case transparency_t::none:
					default:
						break;
					}

				if (create_info.borders == value_t::disable)
					{
					switch (create_info.shadow)
						{
						case value_t::enable : details::style::set_shadow(base.get_handle(), true ); break;
						case value_t::disable: details::style::set_shadow(base.get_handle(), false); break;
						case value_t::_default:
						default:
							break;
						}
					}
				}


		private:
			
			std::optional<LRESULT> procedure(UINT msg, WPARAM wparam, LPARAM lparam)
				{
				switch (msg)
					{
					case WM_NCCALCSIZE:
						if (wparam == TRUE && borders == value_t::disable) {
							auto& params = *reinterpret_cast<NCCALCSIZE_PARAMS*>(lparam);
							details::style::adjust_maximized_client_rect(get_base().get_handle(), params.rgrc[0]);
							return 0;
							}
						break;
					}
				return std::nullopt;
				}

		private:
			value_t borders;
		};
	}