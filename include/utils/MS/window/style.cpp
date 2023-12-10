#include "style.h"

#include "window.h"
#include "details/style.h"

#include "../windows.h"
#include "../cast.h"
#include "details/DWM.h"

namespace utils::MS::window
	{
	void style::create_info::adjust_base_create_info(utils::MS::window::base::create_info& base_create_info) const noexcept
		{
		if (transparency == transparency_t::layered)
			{
			base_create_info.style_ex |= WS_EX_LAYERED;
			}

		if (borders == value_t::disable)
			{
			DWORD style{static_cast<DWORD>(details::style::Style::windowed)};
			base_create_info.style |= style;
			}
		}

	style::style(window::base& base, style::create_info create_info) : module{base}, borders{create_info.borders}
		{
		switch (create_info.transparency)
			{
			case transparency_t::composition_attribute:
				details::make_glass_CompositionAttribute(base.get_handle());
				break;
			case transparency_t::DWM_blurbehind:
				details::make_glass_DWM_BlurBehind      (base.get_handle());
				break;
			case transparency_t::DWM_transparent:
				details::make_transparent_DWM_BlurBehind(base.get_handle());
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

	procedure_result style::procedure(UINT msg, WPARAM wparam, LPARAM lparam)
		{
		switch (msg)
			{
			case WM_NCCALCSIZE:
				if (wparam == TRUE && borders == value_t::disable) {
					auto& params = *reinterpret_cast<NCCALCSIZE_PARAMS*>(lparam);
					if (auto opt{details::style::maximized_client_rect(get_base().get_handle())})
						{
						params.rgrc[0] = cast(*opt);
						}
					return procedure_result::stop(0);
					}
				break;
			}

		return procedure_result::next();
		}
	}