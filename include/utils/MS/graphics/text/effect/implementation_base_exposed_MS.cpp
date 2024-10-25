#pragma once

#include "implementation_base_exposed_MS.h"

namespace utils::MS::graphics::text::effect
	{
	com_effect::com_effect(utils::MS::graphics::text::effect::implementation_base& effect_implementation)
		: effect_implementation_ptr{&effect_implementation} 
		{}

	IFACEMETHODIMP_(unsigned long) com_effect::AddRef()
		{
		return InterlockedIncrement(&ref_count);
		}

	IFACEMETHODIMP_(unsigned long) com_effect::Release()
		{
		unsigned long newCount = InterlockedDecrement(&ref_count);
		if (newCount == 0)
			{
			delete this;
			return 0;
			}

		return newCount;
		}

	IFACEMETHODIMP com_effect::QueryInterface(IID const& riid, void** ppvObject)
		{
		if (__uuidof(IUnknown) == riid)
			{
			*ppvObject = this;
			}
		else
			{
			*ppvObject = NULL;
			return E_FAIL;
			}

		this->AddRef();

		return S_OK;
		}

	implementation_base::implementation_base() :
		com_effect_ptr{utils::MS::raw::graphics::create_com_ptr<com_effect>(*this)}
		{}
	}