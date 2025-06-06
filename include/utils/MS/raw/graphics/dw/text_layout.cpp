#include "text_layout.h"

#include "../../../string.h"

namespace utils::MS::raw::graphics::dw::text_layout
	{
	com_ptr create(winrt::com_ptr<IDWriteFactory> dw_factory, winrt::com_ptr<IDWriteTextFormat> text_format, const std::u16string& string, const utils::math::vec2f& sizes)
		{
		com_ptr ret;


		//TODO cry 
		//Notes:
		//wchar_t (and wstring)'s size is platform dependent.
		//We can assume the platform is Windows since we're using Windows API, but this is still ugly and dirty and I wish there was another way.
		//Windows's wstring is utf16.
		const wchar_t* c_wchar_string{reinterpret_cast<const wchar_t*>(string.c_str())};

		winrt::check_hresult(dw_factory->CreateTextLayout(c_wchar_string, static_cast<uint32_t>(string.size()), text_format.get(), sizes.x(), sizes.y(), ret.put()));
		return ret;
		}
	}