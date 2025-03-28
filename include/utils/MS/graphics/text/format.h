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
	enum class weight
		{
		thin       = 100,
		light      = 300,
		semi_light = 350,
		normal     = 400,
		semi_bold  = 600,
		bold       = 700,
		};
	enum class style        { normal, oblique, italic };
	enum class antialiasing { none, greyscale, cleartype };
	enum class word_wrap    { none, normal, whole_word, character };
	
	struct format
		{
		std::string font{};

		/// <summary> Font size is in dips (see utils::MS::graphics::conversions) </summary>
		float size{16.f};
		utils::alignment alignment{utils::alignment::horizontal::left, utils::alignment::vertical::top};

		weight       weight      {weight      ::normal   };
		style        style       {style       ::normal   };
		antialiasing antialiasing{antialiasing::greyscale};
		word_wrap    word_wrap   {word_wrap   ::normal   };

		std::string locale{"en-gb"};

		bool operator==(const format& other) const noexcept = default;
		};

	namespace regions
		{
		enum class lines_type { one, two, three, squiggly };
		struct lines
			{
			lines_type type{regions::lines_type::one};
			utils::graphics::colour::rgba_f colour;
			};

		struct format
			{
			std::string font{};

			/// <summary> Font size is in dips (see utils::MS::graphics::conversions) </summary>
			float size{16.f};
		
			weight       weight      {weight::normal};
			style        style       {style ::normal};

			std::string locale{"en-gb"};
			bool strikethrough{false};
			bool underline    {false};

			bool operator==(const format& other) const noexcept = default;

			struct regions
				{
				utils::containers::regions<std::string                    > font         ;
				utils::containers::regions<float                          > size         ;
				utils::containers::regions<text::weight                   > weight       ;
				utils::containers::regions<text::style                    > style        ;
				utils::containers::regions<std::string                    > locale       ;
				utils::containers::regions<lines                          > strikethrough;
				utils::containers::regions<lines                          > underline    ;
				utils::containers::regions<utils::graphics::colour::rgba_f> highlight    ;
				};
	
			struct optional
				{
				std::optional<std::string                    > font         {std::nullopt};
				std::optional<float                          > size         {std::nullopt};
				std::optional<text::weight                   > weight       {std::nullopt};
				std::optional<text::style                    > style        {std::nullopt};
				std::optional<std::string                    > locale       {std::nullopt};
				std::optional<lines                          > strikethrough{std::nullopt};
				std::optional<lines                          > underline    {std::nullopt};
				std::optional<utils::graphics::colour::rgba_f> highlight    {std::nullopt};

				bool operator==(const optional& other) const noexcept = default;
				};
	
			using accessors_helper = utils::aggregate::accessors_helper
				<
				[](auto& instance) noexcept -> auto& { return instance.font         ; },
				[](auto& instance) noexcept -> auto& { return instance.size         ; },
				[](auto& instance) noexcept -> auto& { return instance.weight       ; },
				[](auto& instance) noexcept -> auto& { return instance.style        ; },
				[](auto& instance) noexcept -> auto& { return instance.locale       ; },
				[](auto& instance) noexcept -> auto& { return instance.strikethrough; },
				[](auto& instance) noexcept -> auto& { return instance.underline    ; },
				[](auto& instance) noexcept -> auto& { return instance.highlight    ; }
				>;
			};

		struct render_element
			{
			bool                            to_image;
			bool                            to_shapes;
			utils::graphics::colour::rgba_f colour;
			bool operator==(const render_element& other) const noexcept = default;
		
			struct regions
				{
				utils::containers::regions<bool                           > to_image;
				utils::containers::regions<bool                           > to_shapes;
				utils::containers::regions<utils::graphics::colour::rgba_f> colour;
				};
			struct optional
				{
				std::optional<bool                           > to_image {std::nullopt};
				std::optional<bool                           > to_shapes{std::nullopt};
				std::optional<utils::graphics::colour::rgba_f> colour   {std::nullopt};
				bool operator==(const optional& other) const noexcept = default;
				};
			using accessors_helper = utils::aggregate::accessors_helper
				<
				[](auto& instance) noexcept -> auto& { return instance.to_image ; },
				[](auto& instance) noexcept -> auto& { return instance.to_shapes; },
				[](auto& instance) noexcept -> auto& { return instance.colour   ; }
				>;
			};

		struct render
			{
			render_element fill         ;
			render_element outline      ;
			render_element strikethrough;
			render_element underline    ;
			bool operator==(const render& other) const noexcept = default;
		
			struct regions
				{
				render_element::regions fill         ;
				render_element::regions outline      ;
				render_element::regions strikethrough;
				render_element::regions underline    ;
				};
			struct optional
				{
				render_element::optional fill         ;
				render_element::optional outline      ;
				render_element::optional strikethrough;
				render_element::optional underline    ;
				bool operator==(const optional& other) const noexcept = default;
				};
			using accessors_helper = utils::aggregate::accessors_helper
				<
				utils::aggregate::accessors_recursive_helper<[](auto& owner) noexcept -> auto& { return owner.fill         ; }, typename render_element::accessors_helper> {},
				utils::aggregate::accessors_recursive_helper<[](auto& owner) noexcept -> auto& { return owner.outline      ; }, typename render_element::accessors_helper> {},
				utils::aggregate::accessors_recursive_helper<[](auto& owner) noexcept -> auto& { return owner.strikethrough; }, typename render_element::accessors_helper> {},
				utils::aggregate::accessors_recursive_helper<[](auto& owner) noexcept -> auto& { return owner.underline    ; }, typename render_element::accessors_helper> {}
				>;
			};

		struct properties
			{
			format format;
			render render;
			bool operator==(const properties& other) const noexcept = default;
		
			struct regions
				{
				text::regions::format::regions format;
				text::regions::render::regions render;
				};
			struct optional
				{
				text::regions::format::optional format;
				text::regions::render::optional render;
				bool operator==(const optional& other) const noexcept = default;
				};
			using accessors_helper = utils::aggregate::accessors_helper
				<
				utils::aggregate::accessors_recursive_helper<[](auto& owner) noexcept -> auto& { return owner.format; }, typename text::regions::format::accessors_helper> {},
				utils::aggregate::accessors_recursive_helper<[](auto& owner) noexcept -> auto& { return owner.render; }, typename text::regions::render::accessors_helper> {}
				>;
			};
		}
	}