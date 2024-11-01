#pragma once

#include "../../../../graphics/text/effects.h"

#include <Unknwn.h>
#include <winrt/base.h>

#include <optional>

#include <utils/graphics/colour.h>

namespace utils::MS::raw::graphics::text::custom_renderer::effects
	{
	using data     = utils::MS::graphics::text::effects;
	using data_opt = utils::MS::graphics::text::effects_opt;

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