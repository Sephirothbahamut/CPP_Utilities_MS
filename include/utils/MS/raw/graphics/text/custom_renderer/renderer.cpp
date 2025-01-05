#include "renderer.h"

namespace utils::MS::raw::graphics::text::custom_renderer::renderer
	{
	IFACEMETHODIMP_(unsigned long) com_class::AddRef()
		{
		return InterlockedIncrement(&ref_count);
		}

	IFACEMETHODIMP_(unsigned long) com_class::Release()
		{
		unsigned long newCount = InterlockedDecrement(&ref_count);
		if (newCount == 0)
			{
			delete this;
			return 0;
			}

		return newCount;
		}

	IFACEMETHODIMP com_class::QueryInterface(IID const& riid, void** ppvObject)
		{
		if (__uuidof(IDWriteTextRenderer) == riid)
			{
			*ppvObject = this;
			}
		else if (__uuidof(IDWritePixelSnapping) == riid)
			{
			*ppvObject = this;
			}
		else if (__uuidof(IUnknown) == riid)
			{
			*ppvObject = this;
			}
		else
			{
			*ppvObject = NULL;
			return E_FAIL;
			}

		this->AddRef();

		return S_OK;
		}

	com_class::com_class(d2d::factory::com_ptr& d2d_factory) : d2d_factory{d2d_factory} {}

	const effects::data& com_class::get_default_rendering_properties() const noexcept { return default_effects; }
	      effects::data& com_class::get_default_rendering_properties()       noexcept { return default_effects; }

	winrt::com_ptr<ID2D1TransformedGeometry> com_class::evaluate_transformed_geometry(FLOAT baselineOriginX, FLOAT baselineOriginY, DWRITE_GLYPH_RUN const* glyphRun)
		{
		// Create the path geometry.
		winrt::com_ptr<ID2D1PathGeometry> path_geometry;
		winrt::check_hresult(d2d_factory->CreatePathGeometry(path_geometry.put()));

		// Write to the path geometry using the geometry sink.
		winrt::com_ptr<ID2D1GeometrySink> pSink;
		winrt::check_hresult(path_geometry->Open(pSink.put()));

		// Get the glyph run outline geometries back from DirectWrite and place them within the geometry sink.
		winrt::check_hresult(glyphRun->fontFace->GetGlyphRunOutline(
			glyphRun->fontEmSize,
			glyphRun->glyphIndices,
			glyphRun->glyphAdvances,
			glyphRun->glyphOffsets,
			glyphRun->glyphCount,
			glyphRun->isSideways,
			glyphRun->bidiLevel % 2,
			pSink.get()));

		// Close the geometry sink
		winrt::check_hresult(pSink->Close());

		// Initialize a matrix to translate the origin of the glyph run.
		D2D1::Matrix3x2F const matrix{D2D1::Matrix3x2F(
			1.0f, 0.0f,
			0.0f, 1.0f,
			baselineOriginX, baselineOriginY)};

		// Create the transformed geometry
		winrt::com_ptr<ID2D1TransformedGeometry> transformed_geometry;
		winrt::check_hresult(d2d_factory->CreateTransformedGeometry(path_geometry.get(), &matrix, transformed_geometry.put()));
		return transformed_geometry;
		}


	void com_class::outline_to_shapes(const winrt::com_ptr<ID2D1TransformedGeometry>& transformed_geometry, std::vector<shape_outline_t>& shapes_out, std::vector<winrt::com_ptr<ID2D1TransformedGeometry>>& dx_geometries_out)
		{
		//TODO
		//https://learn.microsoft.com/en-us/windows/win32/api/d2d1/nf-d2d1-id2d1geometry-outline(constd2d1_matrix_3x2_f_id2d1simplifiedgeometrysink)
		//transformed_geometry->Outline();
		auto geometry_sink{geometry_sink::create()};

		D2D1::Matrix3x2F const matrix = D2D1::Matrix3x2F(
			1.0f, 0.0f,
			0.0f, 1.0f,
			0.0f, 0.0f);

		transformed_geometry->Simplify(D2D1_GEOMETRY_SIMPLIFICATION_OPTION_CUBICS_AND_LINES, D2D1::Matrix3x2F::Identity(), geometry_sink.get());
		//transformed_geometry->Outline(matrix, geometry_sink.get());
		
		for (const auto& shape : geometry_sink->outlines)
			{
			shapes_out.emplace_back(std::move(shape));
			}
		dx_geometries_out.emplace_back(transformed_geometry);
		}

	IFACEMETHODIMP com_class::DrawGlyphRun(
		__maybenull void* clientDrawingContext,
		FLOAT baselineOriginX,
		FLOAT baselineOriginY,
		DWRITE_MEASURING_MODE measuringMode,
		__in DWRITE_GLYPH_RUN const* glyphRun,
		__in DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription,
		::IUnknown* clientDrawingEffect)
		{
		if (clientDrawingContext == nullptr) { return E_POINTER; }
		contexts& contexts{*reinterpret_cast<custom_renderer::contexts*>(clientDrawingContext)};
		const auto effects{effects::from_iunknown(default_effects, clientDrawingEffect)};


		if (effects.outline.to_image)
			{
			const auto transformed_geometry{evaluate_transformed_geometry(baselineOriginX, baselineOriginY, glyphRun)};

			if (effects.text.to_image)
				{
				const auto brush_fill{d2d::brush::create(contexts.render_context, effects.text.colour)};
				contexts.render_context->FillGeometry(transformed_geometry.get(), brush_fill.get());
				}

			const auto brush_outline{d2d::brush::create(contexts.render_context, effects.outline.colour)};
			contexts.render_context->DrawGeometry(transformed_geometry.get(), brush_outline.get());

			if (effects.outline.to_shapes)
				{
				outline_to_shapes(transformed_geometry, contexts.outlines, contexts.dx_geometries);
				}
			}
		else
			{
			if (effects.text.to_image)
				{
				const auto brush_fill{d2d::brush::create(contexts.render_context, effects.text.colour)};

				contexts.render_context->DrawGlyphRun(D2D1_POINT_2F{baselineOriginX, baselineOriginY}, glyphRun, brush_fill.get(), measuringMode);
				}

			if (effects.outline.to_shapes)
				{
				const auto transformed_geometry{evaluate_transformed_geometry(baselineOriginX, baselineOriginY, glyphRun)};
				outline_to_shapes(transformed_geometry, contexts.outlines, contexts.dx_geometries);
				}
			}

		return S_OK;
		}

	IFACEMETHODIMP com_class::DrawUnderline(
		__maybenull void* clientDrawingContext,
		FLOAT baselineOriginX,
		FLOAT baselineOriginY,
		__in DWRITE_UNDERLINE const* underline,
		::IUnknown* clientDrawingEffect)
		{
		if (clientDrawingContext == nullptr) { return E_POINTER; }
		contexts& contexts{*reinterpret_cast<custom_renderer::contexts*>(clientDrawingContext)};
		const auto effects{effects::from_iunknown(default_effects, clientDrawingEffect)};
		
		if (effects.decorators.to_image)
			{
			D2D1_RECT_F rect{D2D1::RectF(0, underline->offset, underline->width, underline->offset + underline->thickness)};

			winrt::com_ptr<ID2D1RectangleGeometry> rectangle_geometry;
			winrt::check_hresult(d2d_factory->CreateRectangleGeometry(&rect, rectangle_geometry.put()));

			// Initialize a matrix to translate the origin of the underline
			D2D1::Matrix3x2F const matrix = D2D1::Matrix3x2F(
				1.0f, 0.0f,
				0.0f, 1.0f,
				baselineOriginX, baselineOriginY);

			winrt::com_ptr<ID2D1TransformedGeometry> transformed_geometry;
			winrt::check_hresult(d2d_factory->CreateTransformedGeometry(rectangle_geometry.get(), &matrix, transformed_geometry.put()));

			const auto brush{d2d::brush::create(contexts.render_context, effects.decorators.colour)};


			//const auto brush2{d2d::brush::create(contexts.render_context, utils::graphics::colour::rgba_f{1.f, 0.f, 0.f, 1.f})};
			//
			//D2D1_RECT_F rect2
			//	{
			//	baselineOriginX                   , baselineOriginY + underline->offset,
			//	baselineOriginX + underline->width, baselineOriginY + underline->offset
			//	};
			//contexts.render_context->DrawRectangle(rect2, brush2.get(), 2.f);

			// Draw the outline of the rectangle
			contexts.render_context->DrawGeometry(transformed_geometry.get(), brush.get());

			// Fill in the rectangle
			contexts.render_context->FillGeometry(transformed_geometry.get(), brush.get());
			}

		if (effects.decorators.to_shapes)
			{
			contexts.underlines.emplace_back(
				utils::math::vec2f{baselineOriginX                   , baselineOriginY + underline->offset},
				utils::math::vec2f{baselineOriginX + underline->width, baselineOriginY + underline->offset});
			}
		
		return S_OK;
		}

	IFACEMETHODIMP com_class::DrawStrikethrough(
		__maybenull void* clientDrawingContext,
		FLOAT baselineOriginX,
		FLOAT baselineOriginY,
		__in DWRITE_STRIKETHROUGH const* strikethrough,
		::IUnknown* clientDrawingEffect)
		{
		if (clientDrawingContext == nullptr) { return E_POINTER; }
		contexts& contexts{*reinterpret_cast<custom_renderer::contexts*>(clientDrawingContext)};
		const auto effects{effects::from_iunknown(default_effects, clientDrawingEffect)};

		if (effects.decorators.to_image)
			{
			HRESULT hr;
			D2D1_RECT_F rect = D2D1::RectF(0, strikethrough->offset, strikethrough->width, strikethrough->offset + strikethrough->thickness);

			winrt::com_ptr<ID2D1RectangleGeometry> rectangle_geometry;
			hr = d2d_factory->CreateRectangleGeometry(&rect, rectangle_geometry.put());

			// Initialize a matrix to translate the origin of the underline
			D2D1::Matrix3x2F const matrix = D2D1::Matrix3x2F(
				1.0f, 0.0f,
				0.0f, 1.0f,
				baselineOriginX, baselineOriginY);

			winrt::com_ptr<ID2D1TransformedGeometry> transformed_geometry;
			if (SUCCEEDED(hr))
				{
				hr = d2d_factory->CreateTransformedGeometry(rectangle_geometry.get(), &matrix, transformed_geometry.put());
				}

			const auto brush{d2d::brush::create(contexts.render_context, effects.decorators.colour)};

			// Draw the outline of the rectangle
			contexts.render_context->DrawGeometry(transformed_geometry.get(), brush.get());

			// Fill in the rectangle
			contexts.render_context->FillGeometry(transformed_geometry.get(), brush.get());
			}

		if (effects.decorators.to_shapes)
			{
			contexts.underlines.emplace_back(
				utils::math::vec2f{baselineOriginX                       , baselineOriginY + strikethrough->offset},
				utils::math::vec2f{baselineOriginX + strikethrough->width, baselineOriginY + strikethrough->offset});
			}

		return S_OK;
		}

	IFACEMETHODIMP com_class::DrawInlineObject(
		__maybenull void* clientDrawingContext,
		FLOAT originX,
		FLOAT originY,
		IDWriteInlineObject* inlineObject,
		BOOL isSideways,
		BOOL isRightToLeft,
		::IUnknown* clientDrawingEffect)
		{
		// Not implemented
		return E_NOTIMPL;
		}


	IFACEMETHODIMP com_class::IsPixelSnappingDisabled(__maybenull void* clientDrawingContext, __out BOOL* isDisabled)
		{
		*isDisabled = FALSE;
		return S_OK;
		}

	IFACEMETHODIMP com_class::GetCurrentTransform(__maybenull void* clientDrawingContext, __out DWRITE_MATRIX* transform)
		{
		if (clientDrawingContext == nullptr) { return E_POINTER; }
		contexts& contexts{*reinterpret_cast<custom_renderer::contexts*>(clientDrawingContext)};
		
		D2D1_MATRIX_3X2_F d2d_transform;
		contexts.render_context->GetTransform(&d2d_transform);
		transform->m11 = d2d_transform.m11;
		transform->m12 = d2d_transform.m12;
		transform->m21 = d2d_transform.m21;
		transform->m22 = d2d_transform.m22;
		transform->dx  = d2d_transform.dx ;
		transform->dy  = d2d_transform.dy ;

		return S_OK;
		}

	IFACEMETHODIMP com_class::GetPixelsPerDip(__maybenull void* clientDrawingContext, __out FLOAT* pixelsPerDip)
		{
		*pixelsPerDip = 1.0f;
		return S_OK;
		}

	com_ptr create(d2d::factory::com_ptr& d2d_factory)
		{
		return utils::MS::raw::graphics::create_com_ptr<com_class>(d2d_factory);
		}
	}