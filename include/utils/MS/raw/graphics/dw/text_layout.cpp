#include "text_layout.h"

#include "../../../string.h"

namespace utils::MS::raw::graphics::dw
	{
	text_layout::text_layout(IDWriteFactory* dw_factory, IDWriteTextFormat* text_format, const std::string& string, const utils::math::vec2f& sizes) : ms_wrapper{nullptr}
		{
		const auto wstring{utils::MS::string::utf8_to_wide(string)};

		throw_if_failed(dw_factory->CreateTextLayout(wstring.c_str(), static_cast<uint32_t>(wstring.size()), text_format, sizes.x(), sizes.y(), &ptr));
		}
	}