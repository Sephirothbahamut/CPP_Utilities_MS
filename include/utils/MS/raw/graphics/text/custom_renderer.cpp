#include "custom_renderer.h"

#include "../common.h"

#include "../dw/factory.h"

namespace utils::MS::raw::graphics::text
	{
	IFACEMETHODIMP_(unsigned long) i_effect_base::AddRef()
		{
		return InterlockedIncrement(&ref_count);
		}

	IFACEMETHODIMP_(unsigned long) i_effect_base::Release()
		{
		unsigned long newCount = InterlockedDecrement(&ref_count);
		if (newCount == 0)
			{
			delete this;
			return 0;
			}

		return newCount;
		}

	IFACEMETHODIMP i_effect_base::QueryInterface(IID const& riid, void** ppvObject)
		{
		if (__uuidof(IUnknown) == riid)
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





































	IFACEMETHODIMP_(unsigned long) i_custom_renderer::AddRef()
		{
		return InterlockedIncrement(&ref_count);
		}

	IFACEMETHODIMP_(unsigned long) i_custom_renderer::Release()
		{
		unsigned long newCount = InterlockedDecrement(&ref_count);
		if (newCount == 0)
			{
			delete this;
			return 0;
			}

		return newCount;
		}

	IFACEMETHODIMP i_custom_renderer::QueryInterface(IID const& riid, void** ppvObject)
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

	i_custom_renderer::i_custom_renderer(d2d::factory& d2d_factory, d2d::brush& brush) :
		d2d_factory{d2d_factory},
		brush{brush}
		{
		}

	IFACEMETHODIMP i_custom_renderer::DrawGlyphRun(
		__maybenull void* clientDrawingContext,
		FLOAT baselineOriginX,
		FLOAT baselineOriginY,
		DWRITE_MEASURING_MODE measuringMode,
		__in DWRITE_GLYPH_RUN const* glyphRun,
		__in DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription,
		::IUnknown* clientDrawingEffect
	)
		{
		if (clientDrawingEffect != nullptr)
			{
			i_effect_base& effect{*reinterpret_cast<i_effect_base*>(clientDrawingEffect)};
			const auto ret{effect.draw_glyph_run({baselineOriginX, baselineOriginY}, measuringMode, glyphRun, glyphRunDescription)};
			return ret;
			}






		HRESULT hr = S_OK;

		// Create the path geometry.
		ID2D1PathGeometry* pPathGeometry = NULL;
		hr = d2d_factory->CreatePathGeometry(&pPathGeometry);

		// Write to the path geometry using the geometry sink.
		ID2D1GeometrySink* pSink = NULL;
		if (SUCCEEDED(hr))
			{
			hr = pPathGeometry->Open(&pSink);
			}

			// Get the glyph run outline geometries back from DirectWrite and place them within the
			// geometry sink.
		if (SUCCEEDED(hr))
			{
			hr = glyphRun->fontFace->GetGlyphRunOutline(
				glyphRun->fontEmSize,
				glyphRun->glyphIndices,
				glyphRun->glyphAdvances,
				glyphRun->glyphOffsets,
				glyphRun->glyphCount,
				glyphRun->isSideways,
				glyphRun->bidiLevel % 2,
				pSink);
			}

			// Close the geometry sink
		if (SUCCEEDED(hr))
			{
			hr = pSink->Close();
			}

			// Initialize a matrix to translate the origin of the glyph run.
		D2D1::Matrix3x2F const matrix = D2D1::Matrix3x2F(
			1.0f, 0.0f,
			0.0f, 1.0f,
			baselineOriginX, baselineOriginY
		);

		// Create the transformed geometry
		ID2D1TransformedGeometry* pTransformedGeometry = NULL;
		if (SUCCEEDED(hr))
			{
			hr = d2d_factory->CreateTransformedGeometry(
				pPathGeometry,
				&matrix,
				&pTransformedGeometry);
			}

		// Draw the outline of the glyph run
		//pRT_->DrawGeometry(pTransformedGeometry, brush);

		// Fill in the glyph run
		//pRT_->FillGeometry(pTransformedGeometry, brush);

		safe_release(pPathGeometry);
		safe_release(pSink);
		safe_release(pTransformedGeometry);

		return hr;
		}

	IFACEMETHODIMP i_custom_renderer::DrawUnderline(
		__maybenull void* clientDrawingContext,
		FLOAT baselineOriginX,
		FLOAT baselineOriginY,
		__in DWRITE_UNDERLINE const* underline,
		::IUnknown* clientDrawingEffect
	)
		{
		HRESULT hr;

		D2D1_RECT_F rect = D2D1::RectF(
			0,
			underline->offset,
			underline->width,
			underline->offset + underline->thickness
		);

		ID2D1RectangleGeometry* pRectangleGeometry = NULL;
		hr = d2d_factory->CreateRectangleGeometry(
			&rect,
			&pRectangleGeometry
		);

		// Initialize a matrix to translate the origin of the underline
		D2D1::Matrix3x2F const matrix = D2D1::Matrix3x2F(
			1.0f, 0.0f,
			0.0f, 1.0f,
			baselineOriginX, baselineOriginY);

		ID2D1TransformedGeometry* pTransformedGeometry = NULL;
		if (SUCCEEDED(hr))
			{
			hr = d2d_factory->CreateTransformedGeometry(pRectangleGeometry, &matrix, &pTransformedGeometry);
			}

		// Draw the outline of the rectangle
		//pRT_->DrawGeometry(pTransformedGeometry, brush);

		// Fill in the rectangle
		//pRT_->FillGeometry(pTransformedGeometry, brush);

		safe_release(pRectangleGeometry);
		safe_release(pTransformedGeometry);

		return S_OK;
		}

	IFACEMETHODIMP i_custom_renderer::DrawStrikethrough(
		__maybenull void* clientDrawingContext,
		FLOAT baselineOriginX,
		FLOAT baselineOriginY,
		__in DWRITE_STRIKETHROUGH const* strikethrough,
		::IUnknown* clientDrawingEffect
	)
		{
		HRESULT hr;

		D2D1_RECT_F rect = D2D1::RectF(
			0,
			strikethrough->offset,
			strikethrough->width,
			strikethrough->offset + strikethrough->thickness
		);

		ID2D1RectangleGeometry* pRectangleGeometry = NULL;
		hr = d2d_factory->CreateRectangleGeometry(
			&rect,
			&pRectangleGeometry
		);

		// Initialize a matrix to translate the origin of the strikethrough
		D2D1::Matrix3x2F const matrix = D2D1::Matrix3x2F(
			1.0f, 0.0f,
			0.0f, 1.0f,
			baselineOriginX, baselineOriginY
		);

		ID2D1TransformedGeometry* pTransformedGeometry = NULL;
		if (SUCCEEDED(hr))
			{
			hr = d2d_factory->CreateTransformedGeometry(pRectangleGeometry, &matrix, &pTransformedGeometry);
			}

			// Draw the outline of the rectangle
		//pRT_->DrawGeometry(pTransformedGeometry, brush);

		// Fill in the rectangle
		//pRT_->FillGeometry(pTransformedGeometry, brush);

		safe_release(pRectangleGeometry);
		safe_release(pTransformedGeometry);

		return S_OK;
		}

	IFACEMETHODIMP i_custom_renderer::DrawInlineObject(
		__maybenull void* clientDrawingContext,
		FLOAT originX,
		FLOAT originY,
		IDWriteInlineObject* inlineObject,
		BOOL isSideways,
		BOOL isRightToLeft,
		::IUnknown* clientDrawingEffect
	)
		{
			// Not implemented
		return E_NOTIMPL;
		}


	IFACEMETHODIMP i_custom_renderer::IsPixelSnappingDisabled(__maybenull void* clientDrawingContext, __out BOOL* isDisabled)
		{
		*isDisabled = FALSE;
		return S_OK;
		}

	IFACEMETHODIMP i_custom_renderer::GetCurrentTransform(__maybenull void* clientDrawingContext, __out DWRITE_MATRIX* transform)
		{
		//forward the render target's transform
		//pRT_->GetTransform(reinterpret_cast<D2D1_MATRIX_3X2_F*>(transform));
		return S_OK;
		}

	IFACEMETHODIMP i_custom_renderer::GetPixelsPerDip(__maybenull void* clientDrawingContext, __out FLOAT* pixelsPerDip)
		{
		*pixelsPerDip = 1.0f;
		return S_OK;
		}
	}