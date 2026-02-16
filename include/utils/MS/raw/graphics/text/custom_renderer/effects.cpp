#include "effects.h"

#include "../../common.h"

namespace utils::MS::raw::graphics::text::custom_renderer::effects
	{
	utils::MS::graphics::text::regions::format from_iunknown(IUnknown* ptr)
		{
		assert(ptr);

		// Not using this because my class doesn't have an uuid
		// so as<T> that calls QueryInterface can't work
		//const winrt::com_ptr<IUnknown> com_iunknown_ptr{ptr, winrt::take_ownership_from_abi};
		//const auto com_effects_ptr{com_iunknown_ptr.as<effects::com_class>()}; 

		const auto* elements_ptr{dynamic_cast<const effects::com_class*>(ptr)};

		assert(elements_ptr);

		const auto ret{elements_ptr->format_in_region};
		return ret;
		}

	com_class::com_class(const utils::MS::graphics::text::regions::format& format_in_region, const utils::MS::graphics::text::regions::format::regions& regions, const size_t& region_begin) : format_in_region{format_in_region}, regions_ptr{&regions}, region_begin{region_begin} {}

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
	com_ptr create(const utils::MS::graphics::text::regions::format& format_in_region, const utils::MS::graphics::text::regions::format::regions& regions, const size_t& region_begin) 
		{
		return utils::MS::raw::graphics::create_com_ptr<com_class>(format_in_region, regions, region_begin); 
		}
	}