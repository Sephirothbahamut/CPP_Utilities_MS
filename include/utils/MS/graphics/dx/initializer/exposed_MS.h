#pragma once

#include "base.h"

#include "../../../raw/graphics/d2d.h"
#include "../../../raw/graphics/d3d.h"
#include "../../../raw/graphics/dxgi.h"
#include "../../../raw/graphics/dw.h"

namespace utils::MS::graphics::dx
	{
	struct initializer::implementation
		{
		implementation() = default;

		utils::MS::raw::graphics::dw  ::factory::com_ptr dw_factory {utils::MS::raw::graphics::dw  ::factory::create()};
		utils::MS::raw::graphics::d3d ::device ::com_ptr d3d_device {utils::MS::raw::graphics::d3d ::device ::create()};
		utils::MS::raw::graphics::dxgi::device ::com_ptr dxgi_device{utils::MS::raw::graphics::dxgi::device ::create(d3d_device)};
		utils::MS::raw::graphics::d2d ::factory::com_ptr d2d_factory{utils::MS::raw::graphics::d2d ::factory::create()};
		utils::MS::raw::graphics::d2d ::device ::com_ptr d2d_device {utils::MS::raw::graphics::d2d ::device ::create(d2d_factory, dxgi_device)};
		};
	}