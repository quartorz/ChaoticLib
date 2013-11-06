#pragma once

#include <dwrite.h>

#include "Factory.h"
#include "Object.h"
#include "Font.h"
#include "SolidBrush.h"

#include <string>
#include <vector>
#include <utility>

namespace ChaoticLib{ namespace Direct2D{

	class Text: public Object{
	public:
#ifndef DONT_USE_ENUM_CLASS
		enum class Align: unsigned{
			Left = DWRITE_TEXT_ALIGNMENT_LEADING,
			Center = DWRITE_TEXT_ALIGNMENT_CENTER,
			Right = DWRITE_TEXT_ALIGNMENT_TRAILING,
		};
#else
		struct Align{
			static const DWRITE_TEXT_ALIGNMENT Left = DWRITE_TEXT_ALIGNMENT_LEADING;
			static const DWRITE_TEXT_ALIGNMENT Center = DWRITE_TEXT_ALIGNMENT_CENTER;
			static const DWRITE_TEXT_ALIGNMENT Right = DWRITE_TEXT_ALIGNMENT_TRAILING;

			DWRITE_TEXT_ALIGNMENT align;

			Align(DWRITE_TEXT_ALIGNMENT a): align(a)
			{
			}
			operator DWRITE_TEXT_ALIGNMENT()
			{
				return align;
			}
		};
#endif
#ifndef DONT_USE_ENUM_CLASS
		enum class ParagraphAlign: unsigned{
			Top = DWRITE_PARAGRAPH_ALIGNMENT_NEAR,
			Center = DWRITE_PARAGRAPH_ALIGNMENT_CENTER,
			Right = DWRITE_PARAGRAPH_ALIGNMENT_FAR,
		};
#else
		struct ParagraphAlign{
			static const DWRITE_PARAGRAPH_ALIGNMENT Top = DWRITE_PARAGRAPH_ALIGNMENT_NEAR;
			static const DWRITE_PARAGRAPH_ALIGNMENT Center = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
			static const DWRITE_PARAGRAPH_ALIGNMENT Bottom = DWRITE_PARAGRAPH_ALIGNMENT_FAR;

			DWRITE_PARAGRAPH_ALIGNMENT align;

			ParagraphAlign(DWRITE_PARAGRAPH_ALIGNMENT a): align(a)
			{
			}
			operator DWRITE_PARAGRAPH_ALIGNMENT()
			{
				return align;
			}
		};
#endif
#ifndef DONT_USE_ENUM_CLASS
		enum class WordWrapping: unsigned{
			Wrap = DWRITE_WORD_WRAPPING_WRAP,
			NoWrap = DWRITE_WORD_WRAPPING_NO_WRAP,
		};
#else
		struct WordWrapping{
			static const DWRITE_WORD_WRAPPING Wrap = DWRITE_WORD_WRAPPING_WRAP;
			static const DWRITE_WORD_WRAPPING NoWrap = DWRITE_WORD_WRAPPING_NO_WRAP;

			DWRITE_WORD_WRAPPING wordwrapping;

			WordWrapping(DWRITE_WORD_WRAPPING ww): wordwrapping(ww)
			{
			}
			operator DWRITE_WORD_WRAPPING()
			{
				return wordwrapping;
			}
		};
#endif

		typedef Font::FontWeight FontWeight;

	private:
		Factory factory;
		IDWriteTextLayout *layout;
		SolidBrush brush;
		Font font;
		std::wstring text;
		WordWrapping wordwrapping;
		Align align;
		ParagraphAlign p_align;
		std::vector<std::pair<DWRITE_TEXT_RANGE, float>> fontsizes;
		std::vector<std::pair<DWRITE_TEXT_RANGE, bool>> underlines, italics, strikethroughs;
		std::vector<std::pair<DWRITE_TEXT_RANGE, FontWeight>> fontweights;
		std::vector<std::pair<DWRITE_TEXT_RANGE, SolidBrush>> colors;
		bool modified;

		bool Create();
		void SetColors();

	public:
		Text():
			layout(nullptr),
			brush(Color()),
			wordwrapping(WordWrapping::Wrap),
			align(Align::Left),
			p_align(ParagraphAlign::Top),
			modified(true)
		{
		}
		Text(const Text &);
		Text(Text &&);
		~Text();

		void SetFont(const Font &);
		void SetFont(Font &&);
		void SetText(const wchar_t *);
		void SetColor(Color, int start=-1, int end=-1);
		void SetWordWrapping(WordWrapping);
		void SetAlign(Align);
		void SetParagraphAlign(ParagraphAlign);
		void SetUnderline(int start, int end, bool set=true);
		void SetItalic(int start, int end, bool set=true);
		void SetStrikeThrough(int start, int end, bool set=true);

		Font &GetFont();
		const Font &GetFont() const
		{
			return const_cast<Text*>(this)->GetFont();
		}
		const wchar_t *GetText() const;
		Align GetAlign() const;
		ParagraphAlign GetParagraphAlign() const;
		WordWrapping GetWordWrapping() const;

		virtual Rect GetDrawingRect() override;

		virtual bool CreateResource(const CreateStruct &) override;
		virtual void DestroyResource() override;
		virtual void Draw(const PaintStruct &) override;

		IDWriteTextLayout *Get();
	};

} }
