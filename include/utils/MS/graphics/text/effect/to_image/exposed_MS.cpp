#include "base.h"
#include "exposed_MS.h"

#include "../../../dx/initializer/exposed_MS.h"

namespace utils::MS::graphics::text::effect
	{
	to_image::implementation::implementation(
		utils::MS::graphics::dx::initializer & initializer ,
		const utils::math::vec2s             & resolution  ,
		const utils::graphics::colour::rgba_f& clear_colour,
		const utils::graphics::colour::rgba_f& text_colour ) :
		d2d_factory{initializer.implementation_ptr->d2d_factory},
		d2d_context{utils::MS::raw::graphics::d2d::context::create(initializer.implementation_ptr->d2d_device)},
		bitmap{utils::MS::raw::graphics::d2d::bitmap::create(d2d_context, utils::MS::raw::graphics::d2d::bitmap::create_info
			{
			.resolution {resolution},
			.dxgi_format{DXGI_FORMAT_R32G32B32A32_FLOAT},
			.alpha_mode {D2D1_ALPHA_MODE_PREMULTIPLIED},
			.options    {D2D1_BITMAP_OPTIONS_TARGET},
			})},
		brush{utils::MS::raw::graphics::d2d::brush::create(d2d_context, text_colour)}
		{
		d2d_context->SetTarget(bitmap.get());
		d2d_context->BeginDraw();
		d2d_context->Clear(utils::MS::raw::graphics::d2d::cast(clear_colour));
		d2d_context->EndDraw();
		}

		utils::MS::raw::graphics::d2d::factory::com_ptr d2d_factory;
		utils::MS::raw::graphics::d2d::context::com_ptr d2d_context;
		utils::MS::raw::graphics::d2d::brush  ::com_ptr brush;

		HRESULT to_image::implementation::draw_glyph_run(utils::math::vec2f baseline_origin, DWRITE_MEASURING_MODE measuringMode, DWRITE_GLYPH_RUN const* glyphRun, DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription)
			{
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
				baseline_origin.x(), baseline_origin.y()
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
			d2d_context->BeginDraw();
			d2d_context->FillGeometry(pTransformedGeometry, brush.get());
			d2d_context->EndDraw();

			utils::MS::raw::graphics::safe_release(pPathGeometry);
			utils::MS::raw::graphics::safe_release(pSink);
			utils::MS::raw::graphics::safe_release(pTransformedGeometry);

			return hr;
			}

		utils::matrix<utils::graphics::colour::rgba_f> to_image::implementation::get_image()
			{
			const auto cpu_image{utils::MS::raw::graphics::d2d::bitmap::to_cpu_matrix(bitmap, d2d_context)};
			return cpu_image;
			}
		};