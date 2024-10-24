#include "cast.h"

namespace utils::MS::raw::graphics::dw
	{
	DWRITE_TEXT_ALIGNMENT cast(utils::alignment::horizontal alignment) noexcept
		{
		switch (alignment)
			{
			case utils::alignment::horizontal::left  : return DWRITE_TEXT_ALIGNMENT_LEADING ;
			case utils::alignment::horizontal::centre: return DWRITE_TEXT_ALIGNMENT_CENTER  ;
			case utils::alignment::horizontal::right : return DWRITE_TEXT_ALIGNMENT_TRAILING;
			}
		std::unreachable();
		};

	DWRITE_PARAGRAPH_ALIGNMENT cast(utils::alignment::vertical alignment) noexcept
		{
		switch (alignment)
			{
			case utils::alignment::vertical::top   : return DWRITE_PARAGRAPH_ALIGNMENT_NEAR;
			case utils::alignment::vertical::middle: return DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
			case utils::alignment::vertical::bottom: return DWRITE_PARAGRAPH_ALIGNMENT_FAR;
			}
		std::unreachable();
		};

	DWRITE_FONT_WEIGHT cast(utils::MS::graphics::text::weight weight) noexcept
		{
		switch (weight)
			{
			case utils::MS::graphics::text::weight::thin      : return DWRITE_FONT_WEIGHT_THIN      ;
			case utils::MS::graphics::text::weight::light     : return DWRITE_FONT_WEIGHT_LIGHT     ;
			case utils::MS::graphics::text::weight::semi_light: return DWRITE_FONT_WEIGHT_SEMI_LIGHT;
			case utils::MS::graphics::text::weight::normal    : return DWRITE_FONT_WEIGHT_NORMAL    ;
			case utils::MS::graphics::text::weight::semi_bold : return DWRITE_FONT_WEIGHT_SEMI_BOLD ;
			case utils::MS::graphics::text::weight::bold      : return DWRITE_FONT_WEIGHT_BOLD      ;
			}
		std::unreachable();
		}

	DWRITE_FONT_STYLE cast(utils::MS::graphics::text::style style) noexcept
		{
		switch (style)
			{
			case utils::MS::graphics::text::style::normal : return DWRITE_FONT_STYLE_NORMAL ;
			case utils::MS::graphics::text::style::oblique: return DWRITE_FONT_STYLE_OBLIQUE;
			case utils::MS::graphics::text::style::italic : return DWRITE_FONT_STYLE_ITALIC ;
			}
		std::unreachable();
		}
	}