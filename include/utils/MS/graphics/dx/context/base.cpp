#include "base.h"
#include "exposed_MS.h"

#include "../initializer/exposed_MS.h"

namespace utils::MS::graphics::dx
	{
	context::context(initializer& initializer) : 
		implementation_ptr{utils::make_polymorphic_value<context::implementation>(initializer.implementation_ptr->d2d_device)} {};
	context::~context() = default;
	}