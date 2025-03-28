#pragma once

#include <optional>
#include <functional>

#include <utils/memory.h>
#include <utils/math/vec.h>
#include <utils/containers/regions.h>
#include <utils/math/geometry/shape/ab.h>
#include <utils/math/geometry/shape/mixed.h>

#include "common.h"

#include "../../d2d/context.h"
#include "../../../../graphics/text/output.h"

namespace utils::MS::raw::graphics::text::custom_renderer
	{
	struct contexts
		{
		d2d::context::com_ptr render_context;
		utils::MS::graphics::text::output output;
		};
	}