#include "base.h"
#include "exposed_MS.h"

namespace utils::MS::graphics::dx
	{
	initializer::initializer() : implementation_ptr{utils::make_polymorphic_value<initializer::implementation>()} {};
	initializer::~initializer() = default;
	initializer::implementation* initializer::operator->() noexcept { return implementation_ptr.operator->(); }
	}