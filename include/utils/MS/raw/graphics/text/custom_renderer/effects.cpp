#include "effects.h"

#include "../../common.h"

namespace utils::MS::raw::graphics::text::custom_renderer::effects
	{
	data from_iunknown(const data& data_default, IUnknown* ptr)
		{
		if (!ptr) { return data_default; }

		// Not using this because my class doesn't have an uuid
		// so as<T> that calls QueryInterface can't work
		//const winrt::com_ptr<IUnknown> com_iunknown_ptr{ptr, winrt::take_ownership_from_abi};
		//const auto com_effects_ptr{com_iunknown_ptr.as<effects::com_class>()}; 

		const auto* elements_ptr{dynamic_cast<const effects::com_class*>(ptr)};

		if (!elements_ptr) { return data_default; }
		return data_default.apply_opt(elements_ptr->data);
		}

	com_class::com_class(const effects::data_opt& data) : data{data} {}

	IFACEMETHODIMP_(unsigned long) com_class::AddRef()
		{
		return InterlockedIncrement(&ref_count);
		}
	
	IFACEMETHODIMP_(unsigned long) com_class::Release()
		{
		unsigned long newCount = InterlockedDecrement(&ref_count);
		if (newCount == 0)
			{
			delete this;
			return 0;
			}
	
		return newCount;
		}
	
	IFACEMETHODIMP com_class::QueryInterface(IID const& riid, void** ppvObject)
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

	com_ptr create() { return utils::MS::raw::graphics::create_com_ptr<com_class>(); }
	com_ptr create(const data_opt& data) { return utils::MS::raw::graphics::create_com_ptr<com_class>(data); }
	}