#pragma once

#include "DWM.h"
#include "../../cast.h"

namespace utils::MS::window::details::style
	{
	bool composition_enabled() noexcept
		{
		BOOL composition_enabled = FALSE;
		bool success = ::DwmIsCompositionEnabled(&composition_enabled) == S_OK;
		return composition_enabled && success;
		}

	Style select_borderless_style() noexcept
		{
		return composition_enabled() ? Style::aero_borderless : Style::basic_borderless;
		}

	void set_shadow(HWND handle, bool enabled)
		{
		if (composition_enabled())
			{
			static const MARGINS shadow_state[2]{{ 0,0,0,0 },{ 1,1,1,1 }};
			::DwmExtendFrameIntoClientArea(handle, &shadow_state[enabled]);
			}
		}

	auto maximized(HWND hwnd) -> bool
		{
		WINDOWPLACEMENT placement;
		if (!::GetWindowPlacement(hwnd, &placement))
			{
			return false;
			}

		return placement.showCmd == SW_MAXIMIZE;
		}

	std::optional<utils::math::rect<long>> maximized_client_rect(HWND window)
		{
		if (!maximized(window)) { return std::nullopt; }
		auto monitor = ::MonitorFromWindow(window, MONITOR_DEFAULTTONULL);
		if (!monitor) { return std::nullopt; }

		MONITORINFO monitor_info{};
		monitor_info.cbSize = sizeof(monitor_info);
		if (!::GetMonitorInfoW(monitor, &monitor_info)) { return std::nullopt; }

		// when maximized, make the client area fill just the monitor (without task bar) rect,
		// not the whole window rect which extends beyond the monitor.
		return cast(monitor_info.rcWork);
		}
	}