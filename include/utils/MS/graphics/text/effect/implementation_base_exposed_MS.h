#pragma once

#include "../../../raw/graphics/d2d.h"
#include "../../../raw/graphics/d3d.h"
#include "../../../raw/graphics/dxgi.h"
#include "../../../raw/graphics/dw.h"

namespace utils::MS::graphics::text::effect
	{
	struct implementation_base;

	class com_effect : public IUnknown
		{
		private:
			unsigned long ref_count{0};
		public:
			com_effect(implementation_base& effect_implementation);

			IFACEMETHOD_(unsigned long, AddRef) ();
			IFACEMETHOD_(unsigned long, Release) ();
			IFACEMETHOD(QueryInterface) (IID const& riid, void** ppvObject);

			utils::observer_ptr<implementation_base> effect_implementation_ptr{nullptr};
		};

	struct implementation_base
		{
		implementation_base();

		winrt::com_ptr<com_effect> com_effect_ptr;

		virtual HRESULT draw_glyph_run(utils::math::vec2f baseline_origin, DWRITE_MEASURING_MODE measuringMode, DWRITE_GLYPH_RUN const* glyphRun, DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription) = 0;
		};
	}

#ifdef utils_implementation
#include "implementation_base_exposed_MS.cpp"
#endif