#pragma once

#include "common.h"

#include <vector>

namespace utils::MS::raw::graphics::d3d
	{
	struct device : ms_wrapper<ID3D11Device>
		{
		struct create_info
			{
			/// <summary> One or more concatenated values of the enum "D3D11_CREATE_DEVICE_FLAG".</summary>
			UINT flags{D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG};

			/// <summary> In order of preference, from most to least. </summary>
			std::vector<D3D_DRIVER_TYPE> driver_types{D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP, D3D_DRIVER_TYPE_SOFTWARE};
			};

		using ms_wrapper<ID3D11Device>::ms_wrapper;
		device(const create_info& create_info = {});
		};
	}

#ifdef utils_implementation
#include "device.cpp"
#endif