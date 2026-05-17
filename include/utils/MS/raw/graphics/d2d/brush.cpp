#include "brush.h"

#include "context.h"

#include "cast.h"

namespace utils::MS::raw::graphics::d2d
	{
	brush_solid::brush_solid(ID2D1DeviceContext* d2d_context, utils::graphics::colour::rgba_f rgba_f)
		{
		winrt::check_hresult(d2d_context->CreateSolidColorBrush(cast(rgba_f), com_ptr.put()));
		}

	static brush_gradient::create_info create_info_from_simplified(const brush_gradient::create_info_simplified& simplified) noexcept
		{
		std::vector<brush_gradient::create_info::gradient_stop> gradient_stops;
		gradient_stops.reserve(simplified.colours.size());
		for (size_t i = 0; i < simplified.colours.size(); i++)
			{
			const auto& colour{simplified.colours[i]};
			const float percent{static_cast<float>(i) / (simplified.colours.size() - 1)};
			gradient_stops.push_back(brush_gradient::create_info::gradient_stop{.colour{colour}, .percent{percent}});
			}
		return brush_gradient::create_info
			{
			.start_coords  {simplified.start_coords},
			.end_coords    {simplified.end_coords  },
			.gradient_stops{gradient_stops         }
			};
		}

	brush_gradient::brush_gradient(ID2D1DeviceContext* d2d_context, const create_info_simplified& create_info) : 
		brush_gradient{d2d_context, create_info_from_simplified(create_info)}
		{
		}

	brush_gradient::brush_gradient(ID2D1DeviceContext* d2d_context, const create_info& create_info)
		{
		std::vector<D2D1_GRADIENT_STOP> d2d_gradient_stops;
		d2d_gradient_stops.reserve(create_info.gradient_stops.size());
		for (const auto& gradient_stop : create_info.gradient_stops)
			{
			d2d_gradient_stops.push_back(D2D1_GRADIENT_STOP{.position{gradient_stop.percent}, .color{cast(gradient_stop.colour)}});
			}

		winrt::com_ptr<ID2D1GradientStopCollection> collection;
		winrt::check_hresult(d2d_context->CreateGradientStopCollection
			(
			d2d_gradient_stops.data(),
			static_cast<UINT32>(d2d_gradient_stops.size()),
			D2D1_GAMMA_2_2,
			D2D1_EXTEND_MODE_MIRROR,
			collection.put()
			));

		winrt::check_hresult(d2d_context->CreateLinearGradientBrush
			(
			D2D1::LinearGradientBrushProperties(D2D1::Point2F(0, 0), D2D1::Point2F(150, 150)),
			collection.get(),
			com_ptr.put()
			));
		}
	}