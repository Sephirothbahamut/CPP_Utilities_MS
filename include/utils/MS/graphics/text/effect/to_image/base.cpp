#include "base.h"
#include "exposed_MS.h"

namespace utils::MS::graphics::text::effect
	{
	to_image::to_image(
			utils::MS::graphics::dx::initializer & initializer ,
			const utils::math::vec2s             & resolution  ,
			const utils::graphics::colour::rgba_f& clear_colour,
			const utils::graphics::colour::rgba_f& text_colour ):
		base{utils::make_polymorphic_value<effect::implementation_base, to_image::implementation>(initializer, resolution, clear_colour, text_colour)} {};
	to_image::~to_image() = default;

	utils::matrix<utils::graphics::colour::rgba_f> to_image::get_image() 
		{
		auto& as_implementation{dynamic_cast<to_image::implementation&>(*implementation_ptr)};
		return as_implementation.get_image();
		}
	}