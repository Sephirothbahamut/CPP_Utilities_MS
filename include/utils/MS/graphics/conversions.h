#pragma once

#include <utils/math/vec.h>
#include <utils/math/rect.h>

#include <utils/graphics/dpi_conversions.h>

namespace utils::MS::graphics::conversions
	{
	using namespace utils::graphics::dpi_conversions;

	namespace multipliers
		{
		using namespace utils::graphics::dpi_conversions::multipliers;

		inline static constexpr float in_to_dips() noexcept { return 96.f; }
		inline static constexpr float dips_to_in() noexcept { return 1.f / 96.f; }

		inline static constexpr utils::math::vec2f dips_to_px(const utils::math::vec2f& dpi) noexcept { return dips_to_in() * in_to_px(dpi); }
		inline static constexpr utils::math::vec2f px_to_dips(const utils::math::vec2f& dpi) noexcept { return px_to_in(dpi) * in_to_dips(); }

		inline static constexpr float mm_to_dips() noexcept { return mm_to_in() * in_to_dips(); }
		inline static constexpr float dips_to_mm() noexcept { return dips_to_in() * in_to_mm(); }
		}

	utils_gpu_available constexpr auto in_to_dips(const auto& value) { return value * multipliers::in_to_dips(); }
	utils_gpu_available constexpr auto dips_to_in(const auto& value) { return value * multipliers::dips_to_in(); }

	utils_gpu_available constexpr utils::math::vec2f dips_to_px(const auto& value, const utils::math::vec2f& dpi) { return value * multipliers::dips_to_px(dpi); }
	utils_gpu_available constexpr utils::math::vec2f px_to_dips(const auto& value, const utils::math::vec2f& dpi) { return value * multipliers::px_to_dips(dpi); }

	utils_gpu_available constexpr auto mm_to_dips(const auto& value) noexcept { return value * multipliers::mm_to_dips(); }
	utils_gpu_available constexpr auto dips_to_mm(const auto& value) noexcept { return value * multipliers::dips_to_mm(); }

	utils_gpu_available constexpr utils::math::rect<float> px_to_dips(const utils::math::rect<float>& rect, const utils::math::vec2f& dpi) noexcept
		{
		const auto multiplier{utils::MS::graphics::conversions::multipliers::px_to_dips(dpi)};
		return utils::math::rect<float>
			{
			rect.ll() * multiplier.x(),
			rect.up() * multiplier.y(),
			rect.rr() * multiplier.x(),
			rect.dw() * multiplier.y()
			};
		}

	utils_gpu_available constexpr utils::math::rect<float> dips_to_px(const utils::math::rect<float>& rect, const utils::math::vec2f& dpi) noexcept
		{
		const auto multiplier{utils::MS::graphics::conversions::multipliers::dips_to_px(dpi)};
		return utils::math::rect<float>
			{
			rect.ll() * multiplier.x(),
			rect.up() * multiplier.y(),
			rect.rr() * multiplier.x(),
			rect.dw() * multiplier.y()
			};
		}
	}