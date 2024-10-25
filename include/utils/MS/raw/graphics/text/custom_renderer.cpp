#include "custom_renderer.h"

#include "../common.h"

#include "../dw/factory.h"

#include "../../../graphics/text/effect/implementation_base_exposed_MS.h"

namespace utils::MS::raw::graphics::text::custom_renderer
	{
	//IFACEMETHODIMP_(unsigned long) com_effect_base::AddRef()
	//	{
	//	return InterlockedIncrement(&ref_count);
	//	}
	//
	//IFACEMETHODIMP_(unsigned long) com_effect_base::Release()
	//	{
	//	unsigned long newCount = InterlockedDecrement(&ref_count);
	//	if (newCount == 0)
	//		{
	//		delete this;
	//		return 0;
	//		}
	//
	//	return newCount;
	//	}
	//
	//IFACEMETHODIMP com_effect_base::QueryInterface(IID const& riid, void** ppvObject)
	//	{
	//	if (__uuidof(IUnknown) == riid)
	//		{
	//		*ppvObject = this;
	//		}
	//	else
	//		{
	//		*ppvObject = NULL;
	//		return E_FAIL;
	//		}
	//
	//	this->AddRef();
	//
	//	return S_OK;
	//	}

	





































	IFACEMETHODIMP_(unsigned long) com_custom_renderer::AddRef()
		{
		return InterlockedIncrement(&ref_count);
		}

	IFACEMETHODIMP_(unsigned long) com_custom_renderer::Release()
		{
		unsigned long newCount = InterlockedDecrement(&ref_count);
		if (newCount == 0)
			{
			delete this;
			return 0;
			}

		return newCount;
		}

	IFACEMETHODIMP com_custom_renderer::QueryInterface(IID const& riid, void** ppvObject)
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

	com_custom_renderer::com_custom_renderer()
		{
		}

	IFACEMETHODIMP com_custom_renderer::DrawGlyphRun(
		__maybenull void* clientDrawingContext,
		FLOAT baselineOriginX,
		FLOAT baselineOriginY,
		DWRITE_MEASURING_MODE measuringMode,
		__in DWRITE_GLYPH_RUN const* glyphRun,
		__in DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription,
		::IUnknown* clientDrawingEffect)
		{
		if (clientDrawingEffect != nullptr)
			{
			//com_effect_base& effect{*reinterpret_cast<com_effect_base*>(clientDrawingEffect)};
			auto& effect{*reinterpret_cast<utils::MS::graphics::text::effect::com_effect*>(clientDrawingEffect)};
			if (auto* effect_implementation_ptr{effect.effect_implementation_ptr})
				{
				auto& effect_implementation{*effect_implementation_ptr};
				const auto ret{effect_implementation.draw_glyph_run({baselineOriginX, baselineOriginY}, measuringMode, glyphRun, glyphRunDescription)};
				return ret;
				}
			}
		return S_OK;
		}

	IFACEMETHODIMP com_custom_renderer::DrawUnderline(
		__maybenull void* clientDrawingContext,
		FLOAT baselineOriginX,
		FLOAT baselineOriginY,
		__in DWRITE_UNDERLINE const* underline,
		::IUnknown* clientDrawingEffect
	)
		{
		//HRESULT hr;
		//
		//D2D1_RECT_F rect = D2D1::RectF(
		//	0,
		//	underline->offset,
		//	underline->width,
		//	underline->offset + underline->thickness
		//);
		//
		//ID2D1RectangleGeometry* pRectangleGeometry = NULL;
		//hr = d2d_factory->CreateRectangleGeometry(
		//	&rect,
		//	&pRectangleGeometry
		//);
		//
		//// Initialize a matrix to translate the origin of the underline
		//D2D1::Matrix3x2F const matrix = D2D1::Matrix3x2F(
		//	1.0f, 0.0f,
		//	0.0f, 1.0f,
		//	baselineOriginX, baselineOriginY);
		//
		//ID2D1TransformedGeometry* pTransformedGeometry = NULL;
		//if (SUCCEEDED(hr))
		//	{
		//	hr = d2d_factory->CreateTransformedGeometry(pRectangleGeometry, &matrix, &pTransformedGeometry);
		//	}
		//
		//// Draw the outline of the rectangle
		////pRT_->DrawGeometry(pTransformedGeometry, brush);
		//
		//// Fill in the rectangle
		////pRT_->FillGeometry(pTransformedGeometry, brush);
		//
		//safe_release(pRectangleGeometry);
		//safe_release(pTransformedGeometry);

		return S_OK;
		}

	IFACEMETHODIMP com_custom_renderer::DrawStrikethrough(
		__maybenull void* clientDrawingContext,
		FLOAT baselineOriginX,
		FLOAT baselineOriginY,
		__in DWRITE_STRIKETHROUGH const* strikethrough,
		::IUnknown* clientDrawingEffect
	)
		{
		//HRESULT hr;
		//
		//D2D1_RECT_F rect = D2D1::RectF(
		//	0,
		//	strikethrough->offset,
		//	strikethrough->width,
		//	strikethrough->offset + strikethrough->thickness
		//);
		//
		//ID2D1RectangleGeometry* pRectangleGeometry = NULL;
		//hr = d2d_factory->CreateRectangleGeometry(
		//	&rect,
		//	&pRectangleGeometry
		//);
		//
		//// Initialize a matrix to translate the origin of the strikethrough
		//D2D1::Matrix3x2F const matrix = D2D1::Matrix3x2F(
		//	1.0f, 0.0f,
		//	0.0f, 1.0f,
		//	baselineOriginX, baselineOriginY
		//);
		//
		//ID2D1TransformedGeometry* pTransformedGeometry = NULL;
		//if (SUCCEEDED(hr))
		//	{
		//	hr = d2d_factory->CreateTransformedGeometry(pRectangleGeometry, &matrix, &pTransformedGeometry);
		//	}
		//
		//	// Draw the outline of the rectangle
		////pRT_->DrawGeometry(pTransformedGeometry, brush);
		//
		//// Fill in the rectangle
		////pRT_->FillGeometry(pTransformedGeometry, brush);
		//
		//safe_release(pRectangleGeometry);
		//safe_release(pTransformedGeometry);

		return S_OK;
		}

	IFACEMETHODIMP com_custom_renderer::DrawInlineObject(
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


	IFACEMETHODIMP com_custom_renderer::IsPixelSnappingDisabled(__maybenull void* clientDrawingContext, __out BOOL* isDisabled)
		{
		*isDisabled = FALSE;
		return S_OK;
		}

	IFACEMETHODIMP com_custom_renderer::GetCurrentTransform(__maybenull void* clientDrawingContext, __out DWRITE_MATRIX* transform)
		{
		//forward the render target's transform
		//pRT_->GetTransform(reinterpret_cast<D2D1_MATRIX_3X2_F*>(transform));
		return S_OK;
		}

	IFACEMETHODIMP com_custom_renderer::GetPixelsPerDip(__maybenull void* clientDrawingContext, __out FLOAT* pixelsPerDip)
		{
		*pixelsPerDip = 1.0f;
		return S_OK;
		}

	com_ptr create()
		{
		return utils::MS::raw::graphics::create_com_ptr<com_custom_renderer>();
		}
	}