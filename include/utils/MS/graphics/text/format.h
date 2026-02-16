#pragma once

#include <string>
#include <vector>

#include <utils/aggregate.h>
#include <utils/alignment.h>
#include <utils/graphics/colour.h>
#include <utils/containers/regions.h>
#include <utils/containers/aggregate_regions.h>

namespace utils::MS::graphics::text
	{
	//Could use the same values as MS, but magic_enum acts funky with holes in values like this
	//enum class weight
	//	{
	//	thin       = 100,
	//	light      = 300,
	//	semi_light = 350,
	//	normal     = 400,
	//	semi_bold  = 600,
	//	bold       = 700,
	//	};
	enum class weight       { thin, light, semi_light, normal, semi_bold, bold };
	enum class style        { normal, oblique, italic };
	enum class antialiasing { none, greyscale, cleartype };
	enum class word_wrap    { none, normal, whole_word, character };
	
	struct format
		{
		std::u16string font{};
		std::u16string locale{u"en-gb"};

		/// <summary> Font size is in dips (see utils::MS::graphics::conversions) </summary>
		float size{16.f};
		float line_spacing_multiplier{1.f};
		utils::alignment alignment   {utils::alignment::horizontal::left, utils::alignment::vertical::top};
		weight           weight      {weight      ::normal   };
		style            style       {style       ::normal   };
		antialiasing     antialiasing{antialiasing::greyscale};
		word_wrap        word_wrap   {word_wrap   ::normal   };


		bool operator==(const format& other) const noexcept = default;
		};

	namespace regions
		{
		struct optional_coloured
			{
			struct render_targets
				{
				bool image{true };
				bool shape{false};
				
				bool operator==(const render_targets& other) const noexcept = default;

				struct regions
					{
					utils::containers::regions<bool> image;
					utils::containers::regions<bool> shape;
					};
	
				using accessors_helper = utils::aggregate::accessors_helper
					<
					[](auto& instance) noexcept -> auto& { return instance.image; },
					[](auto& instance) noexcept -> auto& { return instance.shape; }
					>;
				};

			bool                            enabled;
			utils::graphics::colour::rgba_f colour;
			render_targets                  render_to;
			
			bool operator==(const optional_coloured& other) const noexcept = default;

			struct regions
				{
				utils::containers::regions<bool                           > enabled;
				utils::containers::regions<utils::graphics::colour::rgba_f> colour;
				typename render_targets::regions                            render_to;
				};
	
			using accessors_helper = utils::aggregate::accessors_helper
				<
				[](auto& instance) noexcept -> auto& { return instance.enabled; },
				[](auto& instance) noexcept -> auto& { return instance.colour ; },
				utils::aggregate::accessors_recursive_helper<[](auto& owner) noexcept -> auto& { return owner.render_to; }, typename render_targets::accessors_helper> {}
				>;
			};

		struct format
			{
			std::u16string font{};
			std::u16string locale{u"en-gb"};

			/// <summary> Font size is in dips (see utils::MS::graphics::conversions) </summary>
			float size{16.f};
		
			weight weight{weight::normal};
			style  style {style ::normal};
			optional_coloured fill         {.enabled{true }, .colour{utils::graphics::colour::base::black}};
			optional_coloured outline      {.enabled{false}, .colour{utils::graphics::colour::base::black}};
			optional_coloured strikethrough{.enabled{false}, .colour{utils::graphics::colour::base::black}};
			optional_coloured underline    {.enabled{false}, .colour{utils::graphics::colour::base::black}};
			optional_coloured highlight    {.enabled{false}, .colour{utils::graphics::colour::base::black}};

			bool operator==(const format& other) const noexcept = default;

			struct regions
				{
				utils::containers::regions<std::u16string> font    ;
				utils::containers::regions<std::u16string> locale  ;
				utils::containers::regions<float         > size    {16.f};
				utils::containers::regions<text::weight  > weight  {weight::normal};
				utils::containers::regions<text::style   > style   {style ::normal};
				typename optional_coloured::regions fill         {.enabled{true }, .colour{utils::graphics::colour::base::black}};
				typename optional_coloured::regions outline      {.enabled{false}, .colour{utils::graphics::colour::base::black}};
				typename optional_coloured::regions strikethrough{.enabled{false}, .colour{utils::graphics::colour::base::black}};
				typename optional_coloured::regions underline    {.enabled{false}, .colour{utils::graphics::colour::base::black}};
				typename optional_coloured::regions highlight    {.enabled{false}, .colour{utils::graphics::colour::base::black}};
				
				
				struct create : utils::oop::non_constructible
					{
					inline static regions from_base_format(const text::format& format)
						{
						const regions ret
							{
							.font  {format.font  },
							.locale{format.locale},
							.size  {format.size  },
							.weight{format.weight},
							.style {format.style }
							};
						return ret;
						}
					};
				};
	
			using accessors_helper = utils::aggregate::accessors_helper
				<
				[](auto& instance) noexcept -> auto& { return instance.font         ; },
				[](auto& instance) noexcept -> auto& { return instance.locale       ; },
				[](auto& instance) noexcept -> auto& { return instance.size         ; },
				[](auto& instance) noexcept -> auto& { return instance.weight       ; },
				[](auto& instance) noexcept -> auto& { return instance.style        ; },
				utils::aggregate::accessors_recursive_helper<[](auto& owner) noexcept -> auto& { return owner.fill         ; }, typename optional_coloured::accessors_helper> {},
				utils::aggregate::accessors_recursive_helper<[](auto& owner) noexcept -> auto& { return owner.outline      ; }, typename optional_coloured::accessors_helper> {},
				utils::aggregate::accessors_recursive_helper<[](auto& owner) noexcept -> auto& { return owner.strikethrough; }, typename optional_coloured::accessors_helper> {},
				utils::aggregate::accessors_recursive_helper<[](auto& owner) noexcept -> auto& { return owner.underline    ; }, typename optional_coloured::accessors_helper> {},
				utils::aggregate::accessors_recursive_helper<[](auto& owner) noexcept -> auto& { return owner.highlight    ; }, typename optional_coloured::accessors_helper> {}
				>;
			};
		}
	}