#include "text_format.h"

#include "cast.h"
#include "../../../string.h"

namespace utils::MS::raw::graphics::dw::text_format
	{
	com_ptr create(winrt::com_ptr<IDWriteFactory> dw_factory, const utils::MS::graphics::text::format& format)
		{
		com_ptr ret;
		const std::wstring font  {utils::MS::string::utf8_to_wide(format.font  )};
		const std::wstring locale{utils::MS::string::utf8_to_wide(format.locale)};

		winrt::check_hresult(dw_factory->CreateTextFormat(
			font.c_str(),                // Font family name.
			NULL,                        // Font collection (NULL sets it to use the system font collection).
			cast(format.weight),
			cast(format.style ),
			DWRITE_FONT_STRETCH_NORMAL,
			format.size,
			locale.c_str(),
			ret.put()));

		winrt::check_hresult(ret->SetTextAlignment     (cast(format.alignment.horizontal)));
		winrt::check_hresult(ret->SetParagraphAlignment(cast(format.alignment.vertical  )));
		return ret;
		}
	}