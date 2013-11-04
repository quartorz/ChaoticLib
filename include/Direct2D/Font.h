#pragma once

#include <dwrite.h>

#include "Resource.h"
#include "Factory.h"

#include <string>

namespace ChaoticLib{ namespace Direct2D{

	class Font: public Resource{
	public:
		// enum class FontWeight: DWRITE_FONT_WEIGHT{
		// 	Light = DWRITE_FONT_WEIGHT_LIGHT,
		// 	Normal = DWRITE_FONT_WEIGHT_NORMAL,
		// 	Bold = DWRITE_FONT_WEIGHT_BOLD,
		// };
		struct FontWeight{
			static const DWRITE_FONT_WEIGHT Light = DWRITE_FONT_WEIGHT_LIGHT;
			static const DWRITE_FONT_WEIGHT Normal = DWRITE_FONT_WEIGHT_NORMAL;
			static const DWRITE_FONT_WEIGHT Bold = DWRITE_FONT_WEIGHT_BOLD;

			DWRITE_FONT_WEIGHT weight;

			FontWeight(DWRITE_FONT_WEIGHT w): weight(w)
			{
			}

			operator DWRITE_FONT_WEIGHT()
			{
				return weight;
			}
		};
		// enum class FontStyle: DWRITE_FONT_STYLE{
		//	Normal,
		//	Italic,
		// };
		struct FontStyle{
			static const DWRITE_FONT_STYLE Normal = DWRITE_FONT_STYLE_NORMAL;
			static const DWRITE_FONT_STYLE Italic = DWRITE_FONT_STYLE_ITALIC;

			DWRITE_FONT_STYLE style;

			FontStyle(DWRITE_FONT_STYLE s): style(s)
			{
			}
			operator DWRITE_FONT_STYLE()
			{
				return style;
			}
		};

	private:
		Factory factory;
		IDWriteTextFormat *format;
		std::wstring locale, name;
		float size;
		FontWeight weight;
		FontStyle style;
		bool modified;

	public:
		Font():
			format(nullptr),
			locale(L"ja-jp"),
			name(L"Meiryo"),
			size(18.f),
			weight(FontWeight::Normal),
			style(FontStyle::Normal),
			modified(true)
		{
		}
		Font(const Font &);
		~Font();

		void SetLocale(const wchar_t *);
		void SetFontName(const wchar_t *);
		void SetFontSize(float);
		void SetFontWeight(FontWeight);
		void SetFontStyle(FontStyle);

		virtual bool CreateResource(const CreateStruct &) override;
		virtual void DestroyResource() override;

		bool Create();
		IDWriteTextFormat *Get();
		IDWriteTextFormat *Get() const;
	};

} }
