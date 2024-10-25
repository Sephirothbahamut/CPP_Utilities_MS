#pragma once

#include "../d2d.h"
#include "../dw.h"


#include <functional>

#include <utils/memory.h>
#include <utils/math/vec2.h>

namespace utils::MS::raw::graphics::text::custom_renderer
	{
	//class com_effect_base : public IUnknown
	//	{
	//	private:
	//		unsigned long ref_count{0};
	//	public:
	//		IFACEMETHOD_(unsigned long, AddRef) ();
	//		IFACEMETHOD_(unsigned long, Release) ();
	//		IFACEMETHOD(QueryInterface) (IID const& riid, void** ppvObject);
	//
	//		virtual HRESULT draw_glyph_run(
	//			utils::math::vec2f baseline_origin,
	//			DWRITE_MEASURING_MODE measuringMode,
	//			__in DWRITE_GLYPH_RUN const* glyphRun,
	//			__in DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription) = 0;
	//	};
	
	class com_custom_renderer : public IDWriteTextRenderer
		{
		private:
			unsigned long ref_count{0};
		public:
			IFACEMETHOD_(unsigned long, AddRef) ();
			IFACEMETHOD_(unsigned long, Release) ();
			IFACEMETHOD(QueryInterface) (IID const& riid, void** ppvObject);

		public:
			com_custom_renderer();

			IFACEMETHOD(IsPixelSnappingDisabled)(__maybenull void* clientDrawingContext, __out BOOL* isDisabled);

			IFACEMETHOD(GetCurrentTransform)(__maybenull void* clientDrawingContext, __out DWRITE_MATRIX* transform);

			IFACEMETHOD(GetPixelsPerDip)(__maybenull void* clientDrawingContext, __out FLOAT* pixelsPerDip);

			IFACEMETHOD(DrawGlyphRun)(
				__maybenull void* clientDrawingContext,
				FLOAT baselineOriginX,
				FLOAT baselineOriginY,
				DWRITE_MEASURING_MODE measuringMode,
				__in DWRITE_GLYPH_RUN const* glyphRun,
				__in DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription,
				::IUnknown* clientDrawingEffect
				);

			IFACEMETHOD(DrawUnderline)(
				__maybenull void* clientDrawingContext,
				FLOAT baselineOriginX,
				FLOAT baselineOriginY,
				__in DWRITE_UNDERLINE const* underline,
				::IUnknown* clientDrawingEffect
				);

			IFACEMETHOD(DrawStrikethrough)(
				__maybenull void* clientDrawingContext,
				FLOAT baselineOriginX,
				FLOAT baselineOriginY,
				__in DWRITE_STRIKETHROUGH const* strikethrough,
				::IUnknown* clientDrawingEffect
				);

			IFACEMETHOD(DrawInlineObject)(
				__maybenull void* clientDrawingContext,
				FLOAT originX,
				FLOAT originY,
				IDWriteInlineObject* inlineObject,
				BOOL isSideways,
				BOOL isRightToLeft,
				::IUnknown* clientDrawingEffect
				);
		};

	using com_ptr = winrt::com_ptr<com_custom_renderer>;
	com_ptr create();
	}

#ifdef utils_implementation
#include "custom_renderer.cpp"
#endif