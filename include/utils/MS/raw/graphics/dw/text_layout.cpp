#include "text_layout.h"

#include "../../../string.h"

namespace utils::MS::raw::graphics::dw::text_layout
	{
	com_ptr create(winrt::com_ptr<IDWriteFactory> dw_factory, winrt::com_ptr<IDWriteTextFormat> text_format, const std::string& string, const utils::math::vec2f& sizes)
		{
		com_ptr ret;
		const auto wstring{utils::MS::string::utf8_to_wide(string)};

		winrt::check_hresult(dw_factory->CreateTextLayout(wstring.c_str(), static_cast<uint32_t>(wstring.size()), text_format.get(), sizes.x(), sizes.y(), ret.put()));
		return ret;
		}
	}