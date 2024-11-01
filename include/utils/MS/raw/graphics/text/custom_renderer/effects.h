#pragma once

#include <Unknwn.h>
#include <winrt/base.h>

#include <optional>

#include <utils/graphics/colour.h>

namespace utils::MS::raw::graphics::text::custom_renderer::effects
	{
	struct data
		{
		bool                            text_to_image       {true              };
		utils::graphics::colour::rgba_f text_colour         {0.f, 0.f, 0.f, 1.f};
		bool                            outline_to_image    {false             };
		bool                            outline_to_shapes   {false             };
		utils::graphics::colour::rgba_f outline_colour      {0.f, 0.f, 0.f, 1.f};
		bool                            decorators_to_image {true              };
		bool                            decorators_to_shapes{false             };
		utils::graphics::colour::rgba_f decorators_colour   {0.f, 0.f, 0.f, 1.f};
		};
	struct data_opt
		{
		std::optional<bool                           > text_to_image       {std::nullopt};
		std::optional<utils::graphics::colour::rgba_f> text_colour         {std::nullopt};
		std::optional<bool                           > outline_to_image    {std::nullopt};
		std::optional<bool                           > outline_to_shapes   {std::nullopt};
		std::optional<utils::graphics::colour::rgba_f> outline_colour      {std::nullopt};
		std::optional<bool                           > decorators_to_image {std::nullopt};
		std::optional<bool                           > decorators_to_shapes{std::nullopt};
		std::optional<utils::graphics::colour::rgba_f> decorators_colour   {std::nullopt};
		};

	data filter_data(const data& base, const data_opt& optional);
	data from_iunknown(const data& data_default, const IUnknown* ptr);

	class com_class : public IUnknown
		{
		private:
			unsigned long ref_count{1};
		public:
			com_class() = default;
			com_class(const data_opt& data);

			IFACEMETHOD_(unsigned long, AddRef) ();
			IFACEMETHOD_(unsigned long, Release) ();
			IFACEMETHOD(QueryInterface) (IID const& riid, void** ppvObject);
			data_opt data;
		};

	using com_ptr = winrt::com_ptr<com_class>;

	com_ptr create();
	com_ptr create(const data_opt& data);
	}

#ifdef utils_implementation
#include "effects.cpp"
#endif