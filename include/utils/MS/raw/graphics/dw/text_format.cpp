#include "text_format.h"

#include "cast.h"
#include "../../../string.h"

namespace utils::MS::raw::graphics::dw
	{
	text_format::text_format(IDWriteFactory* dw_factory, const utils::MS::graphics::text::format& format) : ms_wrapper{nullptr}
		{
		const std::wstring font  {utils::MS::string::utf8_to_wide(format.font  )};
		const std::wstring locale{utils::MS::string::utf8_to_wide(format.locale)};

		throw_if_failed(dw_factory->CreateTextFormat(
			font.c_str(),                // Font family name.
			NULL,                        // Font collection (NULL sets it to use the system font collection).
			cast(format.weight),
			cast(format.style ),
			DWRITE_FONT_STRETCH_NORMAL,
			format.size,
			locale.c_str(),
			&ptr));

		throw_if_failed(ptr->SetTextAlignment     (cast(format.alignment.horizontal)));
		throw_if_failed(ptr->SetParagraphAlignment(cast(format.alignment.vertical  )));
		}
	}