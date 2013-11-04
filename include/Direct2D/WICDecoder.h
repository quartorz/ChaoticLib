#pragma once

#include <wincodec.h>
#include <d2d1.h>
#include <comdef.h>
#include "Factory.h"

namespace ChaoticLib{ namespace Direct2D{

	class WICDecoder{
		Factory factory;
	public:
		HRESULT Decode(LPCWSTR file,ID2D1RenderTarget *target,ID2D1Bitmap **bmp)
		{
			_COM_SMARTPTR_TYPEDEF(IWICBitmapDecoder, __uuidof(IWICBitmapDecoder));
			_COM_SMARTPTR_TYPEDEF(IWICBitmapFrameDecode, __uuidof(IWICBitmapFrameDecode));
			_COM_SMARTPTR_TYPEDEF(IWICFormatConverter, __uuidof(IWICFormatConverter));

			IWICBitmapDecoderPtr decoder;
			::_com_util::CheckError(factory[WIC]->CreateDecoderFromFilename(file,nullptr,GENERIC_READ,WICDecodeMetadataCacheOnLoad,&decoder));

			IWICBitmapFrameDecodePtr frame;
			::_com_util::CheckError(decoder->GetFrame(0,&frame));

			IWICFormatConverterPtr conv;
			::_com_util::CheckError(factory[WIC]->CreateFormatConverter(&conv));

			::_com_util::CheckError(conv->Initialize(frame,GUID_WICPixelFormat32bppPBGRA,WICBitmapDitherTypeNone,nullptr,0.f,WICBitmapPaletteTypeMedianCut));
			::_com_util::CheckError(target->CreateBitmapFromWicBitmap(conv,bmp));
			
			return S_OK;
		}
	};

} }
