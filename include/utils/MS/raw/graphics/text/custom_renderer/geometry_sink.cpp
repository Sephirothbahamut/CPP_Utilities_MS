#include "geometry_sink.h"

namespace utils::MS::raw::graphics::text::custom_renderer::geometry_sink
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
		if (__uuidof(ID2D1SimplifiedGeometrySink) == riid)
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

	void __stdcall com_class::SetFillMode(D2D1_FILL_MODE fillMode)
		{
		//It appears SetFillMode is called once before each separate glyph.
		//I couldn't find any confirmation of this assumption is MS documentation.
		glyphs.emplace_back();
		}
	void __stdcall com_class::SetSegmentFlags(D2D1_PATH_SEGMENT vertexFlags)
		{
		}
	void __stdcall com_class::BeginFigure(D2D1_POINT_2F startPoint, D2D1_FIGURE_BEGIN figureBegin)
		{
		const auto start_point{utils::MS::raw::graphics::d2d::cast(startPoint)};
		current_outline = shape_outline_t{start_point};
		}
	void __stdcall com_class::AddLines(const D2D1_POINT_2F* points, UINT32 pointsCount)
		{
		for (size_t i{0}; i < pointsCount; i++)
			{
			const auto point{utils::MS::raw::graphics::d2d::cast(points[i])};
			current_outline.add_segment/*_cutting_intersection_with_last_element*/(point);
			}
		}
	void __stdcall com_class::AddBeziers(const D2D1_BEZIER_SEGMENT* beziers, UINT32 beziersCount)
		{
		for (size_t i{0}; i < beziersCount; i++)
			{
			const auto bezier{beziers[i]};
			const auto point_1{utils::MS::raw::graphics::d2d::cast(bezier.point1)};
			const auto point_2{utils::MS::raw::graphics::d2d::cast(bezier.point2)};
			const auto point_3{utils::MS::raw::graphics::d2d::cast(bezier.point3)};
			current_outline.add_bezier_4pt/*_cutting_intersection_with_last_element*/(point_1, point_2, point_3);
			}
		}
	void __stdcall com_class::EndFigure(D2D1_FIGURE_END figureEnd)
		{
		current_outline.close();
		glyphs.rbegin()->emplace_back(std::move(current_outline));
		}
	HRESULT __stdcall com_class::Close()
		{
		return E_NOTIMPL;
		}


	com_ptr create() { return utils::MS::raw::graphics::create_com_ptr<com_class>(); }
	}