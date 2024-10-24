#pragma once

#include "../d2d.h"
#include "../dw.h"


namespace utils::MS::raw::graphics::text
	{
	class i_effect_base : public IUnknown
		{
		private:
			unsigned long ref_count{0};
		public:
			IFACEMETHOD_(unsigned long, AddRef) ();
			IFACEMETHOD_(unsigned long, Release) ();
			IFACEMETHOD(QueryInterface) (IID const& riid, void** ppvObject);

			virtual HRESULT draw_glyph_run(
				utils::math::vec2f baseline_origin,
				DWRITE_MEASURING_MODE measuringMode,
				__in DWRITE_GLYPH_RUN const* glyphRun,
				__in DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription) = 0;
		};
	
	class i_custom_renderer : public IDWriteTextRenderer
		{
		private:
			unsigned long ref_count{0};
		public:
			IFACEMETHOD_(unsigned long, AddRef) ();
			IFACEMETHOD_(unsigned long, Release) ();
			IFACEMETHOD(QueryInterface) (IID const& riid, void** ppvObject);

		public:
			i_custom_renderer(d2d::factory& d2d_factory, d2d::brush& brush);

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

		private:
			winrt::com_ptr<ID2D1Factory1       > d2d_factory;
			winrt::com_ptr<ID2D1SolidColorBrush> brush;
		};

	struct custom_renderer : winrt::com_ptr<i_custom_renderer>
		{
		using ms_wrapper<i_custom_renderer>::ms_wrapper;
		custom_renderer(ID2D1Factory1* d2d_factory, ID2D1SolidColorBrush* brush);

		utils::matrix<utils::graphics::colour::rgba_f> to_cpu_matrix(ID2D1DeviceContext* d2d_context);
		};
	}

#ifdef utils_implementation
#include "custom_renderer.cpp"
#endif