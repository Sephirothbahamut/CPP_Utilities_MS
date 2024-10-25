#include "device.h"

namespace utils::MS::raw::graphics::d3d::device
	{
	com_ptr create(const create_info& create_info)
		{
		com_ptr ret;

		D3D_FEATURE_LEVEL feature_level_created;

		HRESULT last{S_FALSE};
		for (const auto& attempt : create_info.driver_types)
			{
			last = D3D11CreateDevice(
				nullptr,                  // specify null to use the default adapter
				attempt,
				0,
				create_info.flags,           // optionally set debug and Direct2D compatibility flags
				nullptr,                  // use the lastest feature level
				0,                        // use the lastest feature level
				D3D11_SDK_VERSION,
				ret.put(), // returns the Direct3D device created
				&feature_level_created,   // returns feature level of device created
				nullptr);
			if (succeeded(last)) { break; }
			}
		winrt::check_hresult(last);

		return ret;
		}
	}