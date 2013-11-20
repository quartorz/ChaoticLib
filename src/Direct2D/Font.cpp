#include "Font.h"
#include "..\Macro.h"

namespace ChaoticLib{ namespace Direct2D{

	Font::Font(const Font &f):
		format(nullptr),
		locale(f.locale),
		name(f.name),
		size(f.size),
		weight(f.weight),
		style(f.style),
		modified(true)
	{
	}

	Font::~Font()
	{
		SafeRelease(format);
	}

	void Font::SetFontName(const wchar_t *f)
	{
		modified = true;
		name = f;
	}

	void Font::SetFontSize(float s)
	{
		modified = true;
		size = s;
	}

	void Font::SetFontWeight(FontWeight w)
	{
		weight = w;
	}

	void Font::SetFontStyle(FontStyle s)
	{
		style = s;
	}

	bool Font::CreateResource(const CreateStruct &)
	{
		return Create();
	}

	void Font::DestroyResource()
	{
		modified = true;
		SafeRelease(format);
	}

	bool Font::Create()
	{
		if(!modified)
			return true;

		if(SUCCEEDED(factory[DWrite]->CreateTextFormat(
			name.c_str(),
			NULL,
			static_cast<DWRITE_FONT_WEIGHT>(weight),
			static_cast<DWRITE_FONT_STYLE>(style),
			DWRITE_FONT_STRETCH_NORMAL,
			size,
			locale.c_str(),
			&format)))
		{
			modified = false;
			return true;
		}

		return false;
	}

	IDWriteTextFormat *Font::Get()
	{
		if(modified){
			SafeRelease(format);
			Create();
		}

		return format;
	}

	IDWriteTextFormat *Font::Get() const
	{
		return const_cast<Font*>(this)->Get();
	}

} }
