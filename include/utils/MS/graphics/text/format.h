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
			bool                            enabled;
			utils::graphics::colour::rgba_f colour ;
			
			bool operator==(const optional_coloured& other) const noexcept = default;

			struct regions
				{
				utils::containers::regions<bool                           > enabled;
				utils::containers::regions<utils::graphics::colour::rgba_f> colour ;
				};
	
			using accessors_helper = utils::aggregate::accessors_helper
				<
				[](auto& instance) noexcept -> auto& { return instance.enabled; },
				[](auto& instance) noexcept -> auto& { return instance.colour ; }
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

		struct render_element
			{
			bool to_image {true };
			bool to_shapes{false};
			bool operator==(const render_element& other) const noexcept = default;
		
			struct regions
				{
				utils::containers::regions<bool> to_image {true };
				utils::containers::regions<bool> to_shapes{false};
				};
			using accessors_helper = utils::aggregate::accessors_helper
				<
				[](auto& instance) noexcept -> auto& { return instance.to_image ; },
				[](auto& instance) noexcept -> auto& { return instance.to_shapes; }
				>;
			};

		struct render
			{
			render_element fill         ;
			render_element outline      ;
			render_element rect         ;
			render_element strikethrough;
			render_element underline    ;
			bool operator==(const render& other) const noexcept = default;
		
			struct regions
				{
				render_element::regions fill         ;
				render_element::regions outline      ;
				render_element::regions rect         ;
				render_element::regions strikethrough;
				render_element::regions underline    ;
				};
			using accessors_helper = utils::aggregate::accessors_helper
				<
				utils::aggregate::accessors_recursive_helper<[](auto& owner) noexcept -> auto& { return owner.fill         ; }, typename render_element::accessors_helper> {},
				utils::aggregate::accessors_recursive_helper<[](auto& owner) noexcept -> auto& { return owner.outline      ; }, typename render_element::accessors_helper> {},
				utils::aggregate::accessors_recursive_helper<[](auto& owner) noexcept -> auto& { return owner.rect         ; }, typename render_element::accessors_helper> {},
				utils::aggregate::accessors_recursive_helper<[](auto& owner) noexcept -> auto& { return owner.strikethrough; }, typename render_element::accessors_helper> {},
				utils::aggregate::accessors_recursive_helper<[](auto& owner) noexcept -> auto& { return owner.underline    ; }, typename render_element::accessors_helper> {}
				>;
			};

		struct properties
			{
			format format{};
			render render{};
			bool operator==(const properties& other) const noexcept = default;
		
			struct regions
				{
				text::regions::format::regions format;
				text::regions::render::regions render;

				struct create : utils::oop::non_constructible
					{
					inline static regions from_base_format(const text::format& format)
						{
						const regions ret
							{
							.format
								{
								.font  {format.font  },
								.locale{format.locale},
								.size  {format.size  },
								.weight{format.weight},
								.style {format.style }
								},
							.render{}
							};
						return ret;
						}
					};
				};
			using accessors_helper = utils::aggregate::accessors_helper
				<
				utils::aggregate::accessors_recursive_helper<[](auto& owner) noexcept -> auto& { return owner.format; }, typename text::regions::format::accessors_helper> {},
				utils::aggregate::accessors_recursive_helper<[](auto& owner) noexcept -> auto& { return owner.render; }, typename text::regions::render::accessors_helper> {}
				>;
			};
		}
	}