#include "effects.h"

#include "../../common.h"

namespace utils::MS::raw::graphics::text::custom_renderer::effects
	{
	utils::MS::graphics::text::regions::properties from_iunknown(const utils::MS::graphics::text::regions::properties& default_properties, IUnknown* ptr)
		{
		if (!ptr) { return default_properties; }

		// Not using this because my class doesn't have an uuid
		// so as<T> that calls QueryInterface can't work
		//const winrt::com_ptr<IUnknown> com_iunknown_ptr{ptr, winrt::take_ownership_from_abi};
		//const auto com_effects_ptr{com_iunknown_ptr.as<effects::com_class>()}; 

		const auto* elements_ptr{dynamic_cast<const effects::com_class*>(ptr)};

		if (!elements_ptr) { return default_properties; }
		using aggregate_regions_t = utils::containers::aggregate_regions<utils::MS::graphics::text::regions::properties, utils::MS::graphics::text::regions::properties::optional, utils::MS::graphics::text::regions::properties::regions, utils::MS::graphics::text::regions::properties::accessors_helper, true>;
		const aggregate_regions_t aggregate_regions{*elements_ptr->properties_regions_ptr};

		const auto ret{aggregate_regions.at(elements_ptr->region_begin, default_properties)};
		return ret;
		}

	com_class::com_class(const utils::MS::graphics::text::regions::properties::regions& properties_regions, const size_t& region_begin) : properties_regions_ptr{&properties_regions}, region_begin{region_begin} {}

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
	com_ptr create(const utils::MS::graphics::text::regions::properties::regions& properties_regions, const size_t& region_begin) { return utils::MS::raw::graphics::create_com_ptr<com_class>(properties_regions, region_begin); }
	}