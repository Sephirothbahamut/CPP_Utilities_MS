#pragma once

#include "common.h"
#include "device.h"

#include <utils/math/vec.h>

struct ID3D11Device;

namespace utils::MS::raw::graphics::d2d
	{
	struct device;
	}

namespace utils::MS::raw::graphics::dxgi
	{
	class swap_chain : public com_wrapper<IDXGISwapChain1>
		{
		public:

			swap_chain(const dxgi::device& dxgi_device, HWND hwnd);
			//temporary flag for composition mode
			swap_chain(const dxgi::device& dxgi_device, HWND hwnd, nullptr_t);

			void resize(utils::math::vec2u size);

			void present() const;
		};
	}

#ifdef utils_implementation
#include "swap_chain.cpp"
#endif