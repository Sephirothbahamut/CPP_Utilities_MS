#pragma once

#include "base.h"
#include "../implementation_base_exposed_MS.h"

#include "../../../../raw/graphics/d2d.h"
#include "../../../../raw/graphics/d3d.h"
#include "../../../../raw/graphics/dxgi.h"
#include "../../../../raw/graphics/dw.h"

namespace utils::MS::graphics::text::effect
	{
	struct to_image::implementation : effect::implementation_base
		{
		implementation(
			utils::MS::graphics::dx::initializer & initializer,
			const utils::math::vec2s             & resolution ,
			const utils::graphics::colour::rgba_f& clear_colur,
			const utils::graphics::colour::rgba_f& text_colour);

		utils::MS::raw::graphics::d2d::factory::com_ptr d2d_factory;
		utils::MS::raw::graphics::d2d::context::com_ptr d2d_context;
		utils::MS::raw::graphics::d2d::bitmap ::com_ptr bitmap;
		utils::MS::raw::graphics::d2d::brush  ::com_ptr brush;

		virtual HRESULT draw_glyph_run(utils::math::vec2f baseline_origin, DWRITE_MEASURING_MODE measuringMode, DWRITE_GLYPH_RUN const* glyphRun, DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription) final override;

		utils::matrix<utils::graphics::colour::rgba_f> get_image();
		};
	}

#ifdef utils_implementation
#include "exposed_MS.cpp"
#endif