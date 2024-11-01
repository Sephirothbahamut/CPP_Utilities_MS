#pragma once

#include "../../d2d.h"
#include "common.h"

#include <vector>

namespace utils::MS::raw::graphics::text::custom_renderer::geometry_sink
	{
	class com_class : public ID2D1SimplifiedGeometrySink
		{
		private:
			unsigned long ref_count{1};
		public:
			IFACEMETHOD_(unsigned long, AddRef) ();
			IFACEMETHOD_(unsigned long, Release) ();
			IFACEMETHOD(QueryInterface) (IID const& riid, void** ppvObject);

			// Inherited via ID2D1SimplifiedGeometrySink
			void __stdcall SetFillMode(D2D1_FILL_MODE fillMode) override;
			void __stdcall SetSegmentFlags(D2D1_PATH_SEGMENT vertexFlags) override;
			void __stdcall BeginFigure(D2D1_POINT_2F startPoint, D2D1_FIGURE_BEGIN figureBegin) override;
			void __stdcall AddLines(const D2D1_POINT_2F* points, UINT32 pointsCount) override;
			void __stdcall AddBeziers(const D2D1_BEZIER_SEGMENT* beziers, UINT32 beziersCount) override;
			void __stdcall EndFigure(D2D1_FIGURE_END figureEnd) override;
			HRESULT __stdcall Close() override;

			std::vector<custom_renderer::shape_outline_t> outlines;
			custom_renderer::shape_outline_t current_outline;
		};

	using com_ptr = winrt::com_ptr<com_class>;

	com_ptr create();
	}

#ifdef utils_implementation
#include "geometry_sink.cpp"
#endif