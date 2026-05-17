#pragma once

#include "common.h"
#include <utils/math/vec.h>

namespace utils::MS::raw::graphics::dxgi
	{
	class swap_chain : public com_wrapper<IDXGISwapChain1>
		{
		public:

			swap_chain(IDXGIDevice3* dxgi_device, HWND hwnd);
			//temporary flag for composition mode
			swap_chain(IDXGIDevice3* dxgi_device, HWND hwnd, nullptr_t);

			void resize(utils::math::vec2u size);

			void present() const;
		};
	}

#ifdef utils_implementation
#include "swap_chain.cpp"
#endif