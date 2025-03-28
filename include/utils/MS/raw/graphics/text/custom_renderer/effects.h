#pragma once

#include "../../../../graphics/text/format.h"

#include <Unknwn.h>
#include <winrt/base.h>

#include <optional>

#include <utils/graphics/colour.h>

namespace utils::MS::raw::graphics::text::custom_renderer::effects
	{
	utils::MS::graphics::text::regions::properties from_iunknown(const utils::MS::graphics::text::regions::properties& data_default, const IUnknown* ptr);

	class com_class : public IUnknown
		{
		private:
			unsigned long ref_count{1};
		public:
			com_class() = default;
			com_class(const utils::MS::graphics::text::regions::properties::optional& data);

			IFACEMETHOD_(unsigned long, AddRef) ();
			IFACEMETHOD_(unsigned long, Release) ();
			IFACEMETHOD(QueryInterface) (IID const& riid, void** ppvObject);
			utils::MS::graphics::text::regions::properties::optional data;
		};

	using com_ptr = winrt::com_ptr<com_class>;

	com_ptr create();
	com_ptr create(const utils::MS::graphics::text::regions::properties::optional& data);
	}

#ifdef utils_implementation
#include "effects.cpp"
#endif