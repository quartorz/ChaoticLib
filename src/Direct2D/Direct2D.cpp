#include <stdexcept>

#include "Factory.h"
#include "BaseTypes.h"
#include "Brush.h"

#pragma comment(lib, "d2d1")
#pragma comment(lib, "dwrite")
#pragma comment(lib, "dwmapi")
#pragma comment(lib, "uxtheme")

namespace ChaoticLib{ namespace Direct2D{

	Factory *_factory = nullptr;

	ID2D1Factory *Direct2D::Factory::factory = nullptr;
	IDWriteFactory *Direct2D::Factory::dwfactory = nullptr;
	IWICImagingFactory *Direct2D::Factory::wicfactory = nullptr;

	bool Initialize()
	{
		try{
			_factory = new Factory(Factory::Init());
		}catch(std::bad_alloc &){
			return false;
		}

		return true;
	}

	void Uninitialize()
	{
		if(_factory != NULL){
			delete _factory;
			_factory = nullptr;
		}
	}

} }
