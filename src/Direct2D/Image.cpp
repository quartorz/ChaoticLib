#include "Image.h"
#include "..\Macro.h"

using namespace ChaoticLib::Direct2D;

void Image::CalcRectangle()
{
	if(clippingrect.Width() <= 0.f || clippingrect.Height() <= 0.f){
		auto s = bmp->GetSize();
		src = Rect(0.f, 0.f, s.width, s.height);
	}else
		src = clippingrect;

	if(size.width == 0.f || size.height == 0.f){
		auto s = bmp->GetSize();
		dest = Rect(pos, Size(s.width, s.height));
	}else
		dest = Rect(pos, size);

	switch(drawingmode){
	case DrawingMode::Stretch:
		break;
	case DrawingMode::Squeeze:
		if(size.width / size.height < src.Width() / src.Height()){
			float x = src.Height() * size.width / src.Width();
			dest.top = (dest.bottom - x) / 2;
			dest.bottom = dest.top + x;
		}else{
			float x = src.Width() * size.height / src.Height();
			dest.left = (dest.right - x) / 2;
			dest.right = dest.left + x;
		}
		break;
	case DrawingMode::Trim:
		if(src.Width() / src.Height() < size.width / size.height){
			float x = size.height * src.Width() / size.width;
			src.top = (src.bottom - x) / 2;
			src.bottom = src.top + x;
		}else{
			float x = size.width * src.Height() / size.height;
			src.left = (src.right - x) / 2;
			src.right = src.left + x;
		}
		break;
	default:
		__assume(0);
	}
}

Image::Image(const Image &i):
	bmp(nullptr),
	filename(i.filename),
	drawingmode(i.drawingmode),
	interpolationmode(i.interpolationmode),
	modified(true)
{
}

Image::~Image()
{
	SafeRelease(bmp);
}

void Image::SetDrawingMode(DrawingMode mode)
{
	drawingmode = mode;

	if(bmp != nullptr)
		CalcRectangle();
}

void Image::SetInterpolationMode(InterpolationMode im)
{
	interpolationmode = im;
}

void Image::SetFileName(const wchar_t *f)
{
	modified = true;
	filename = f;
}

void Image::SetClippingRect(Rect r)
{
	clippingrect = r;

	if(bmp != nullptr)
		CalcRectangle();
}

void Image::SetPosition(const Point &p)
{
	Object::SetPosition(p);

	if(bmp != nullptr)
		CalcRectangle();
}

void Image::SetSize(const Size &s)
{
	Object::SetSize(s);

	if(bmp != nullptr)
		CalcRectangle();
}

bool Image::CreateResource(const CreateStruct &cs)
{
	return Create(cs.target);
}

void Image::DestroyResource()
{
	SafeRelease(bmp);
}

void Image::Draw(const PaintStruct &ps)
{
	if(modified){
		DestroyResource();
		Create(ps.target);
	}

	ps.target->DrawBitmap(
		bmp,
		dest,
		1.f,
		static_cast<D2D1_BITMAP_INTERPOLATION_MODE>(interpolationmode),
		src);
}

bool Image::Create(ID2D1RenderTarget *t)
{
	if(!modified)
		return true;

	modified = false;

	SafeRelease(bmp);

	if(FAILED(decoder.Decode(
		filename.c_str(),
		t,
		&bmp)))
		return false;

	CalcRectangle();

	return true;
}

ID2D1Bitmap *Image::Get() const
{
	return bmp;
}
