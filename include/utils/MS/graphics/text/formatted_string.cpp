#include "formatted_string.h"

#include "../../raw/graphics/dw/text_layout.h"
#include "../../raw/graphics/dw/text_format.h"
#include "../../raw/graphics/dw/factory.h"

namespace utils::MS::graphics::text
	{
	struct formatted_string::implementation : utils::oop::non_copyable, utils::oop::non_movable
		{
		utils::MS::raw::graphics::dw::factory     dw_factory{nullptr};
		utils::MS::raw::graphics::dw::text_layout text_layout{nullptr};
		utils::MS::raw::graphics::dw::text_format dw_format{nullptr};
		utils::MS::raw::graphics::dw::text_layout dw_layout{nullptr};

		implementation() = default;
		implementation(const std::string& string, const format& format, const utils::math::vec2f& sizes) :
			dw_factory{},
			dw_format{dw_factory, format},
			dw_layout{dw_factory, dw_format, string, sizes}
			{
			}

		};


	formatted_string::formatted_string() : implementation_ptr{std::make_unique<formatted_string::implementation>()} {}
	formatted_string::formatted_string(const std::string& string, const format& format, const utils::math::vec2f& sizes) :
		implementation_ptr{std::make_unique<formatted_string::implementation>(string, format, sizes)} {}

	formatted_string::~formatted_string() = default;
	}
