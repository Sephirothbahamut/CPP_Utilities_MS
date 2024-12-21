#pragma once

#include "../../dw.h"
#include "../../d2d.h"

#include "effects.h"
#include "contexts.h"
#include "geometry_sink.h"

namespace utils::MS::raw::graphics::text::custom_renderer::renderer
	{
	class com_class : public IDWriteTextRenderer
		{
		private:
			unsigned long ref_count{1};
			d2d::factory::com_ptr d2d_factory;

			winrt::com_ptr<ID2D1TransformedGeometry> evaluate_transformed_geometry(FLOAT baselineOriginX, FLOAT baselineOriginY, DWRITE_GLYPH_RUN const* glyphRun);
			void outline_to_shapes(const winrt::com_ptr<ID2D1TransformedGeometry>& transformed_geometry, std::vector<shape_outline_t>& shapes_out, std::vector<winrt::com_ptr<ID2D1TransformedGeometry>>& dx_geometries_out);

		public:
			effects::data default_effects;

			IFACEMETHOD_(unsigned long, AddRef ) ();
			IFACEMETHOD_(unsigned long, Release) ();
			IFACEMETHOD(QueryInterface) (IID const& riid, void** ppvObject);

		public:
			com_class(d2d::factory::com_ptr& d2d_factory);
			const effects::data& get_default_rendering_properties() const noexcept;
			      effects::data& get_default_rendering_properties()       noexcept;

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

	using com_ptr = winrt::com_ptr<com_class>;
	com_ptr create(d2d::factory::com_ptr& d2d_factory);
	}

#ifdef utils_implementation
#include "renderer.cpp"
#endif