#pragma once

#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>
#include <comdef.h>

namespace{
	struct D2D{
	}D2D;
	struct DWrite{
	}DWrite;
	struct WIC{
	}WIC;
}

namespace ChaoticLib{ namespace Direct2D{

	class Factory{
		static ID2D1Factory *factory;
		static IDWriteFactory *dwfactory;
		static IWICImagingFactory *wicfactory;
	public:
		struct Init{
		};

		Factory()
		{
			factory->AddRef();
			dwfactory->AddRef();
			wicfactory->AddRef();
		}
		Factory(Init&)
		{
			using namespace _com_util;
			CheckError(::D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &factory));
			CheckError(::DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(dwfactory), reinterpret_cast<IUnknown**>(&dwfactory)));
			CheckError(::CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wicfactory)));
		}
		Factory(const Factory &)
		{
			factory->AddRef();
			dwfactory->AddRef();
			wicfactory->AddRef();
		}
		Factory(Factory &&)
		{
			factory->AddRef();
			dwfactory->AddRef();
			wicfactory->AddRef();
		}
		~Factory()
		{
			factory->Release();
			dwfactory->Release();
			wicfactory->Release();
		}
		ID2D1Factory *operator[](struct D2D&) const
		{
			return factory;
		}
		IDWriteFactory *operator[](struct DWrite&) const
		{
			return dwfactory;
		}
		IWICImagingFactory *operator[](struct WIC&) const
		{
			return wicfactory;
		}
	};

	extern Factory *_factory;

} }
