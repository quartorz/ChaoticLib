#include "Text.h"

#include <algorithm>
#include <functional>

using namespace ChaoticLib::Direct2D;

namespace{
	DWRITE_TEXT_RANGE CreateRange(int start, int end)
	{
		DWRITE_TEXT_RANGE r = {
			start,
			end - start
		};
		return r;
	}
}

bool Text::Create()
{
	if(!modified)
		return true;

	modified = false;

	font.DestroyResource();
	SafeRelease(layout);

	if(!font.Create())
		return false;

	if(FAILED(factory[DWrite]->CreateTextLayout(
		text.c_str(),
		text.length(),
		font.Get(),
		size.width,
		size.height,
		&layout)))
		return false;

	layout->SetTextAlignment(align);
	layout->SetParagraphAlignment(p_align);
	layout->SetWordWrapping(wordwrapping);

	std::for_each(
		fontsizes.begin(), fontsizes.end(),
		[this](std::pair<DWRITE_TEXT_RANGE, float> &p){
			layout->SetFontSize(p.second, p.first);
	});
	std::for_each(
		underlines.begin(), underlines.end(),
		[this](std::pair<DWRITE_TEXT_RANGE, bool> &p){
			layout->SetUnderline(p.second, p.first);
	});
	std::for_each(
		italics.begin(), italics.end(),
		[this](std::pair<DWRITE_TEXT_RANGE, bool> &p){
			layout->SetFontStyle((p.second? DWRITE_FONT_STYLE_ITALIC: DWRITE_FONT_STYLE_NORMAL), p.first);
	});
	std::for_each(
		strikethroughs.begin(), strikethroughs.end(),
		[this](std::pair<DWRITE_TEXT_RANGE, bool> &p){
			layout->SetStrikethrough(p.second, p.first);
	});
	std::for_each(
		fontweights.begin(), fontweights.end(),
		[this](std::pair<DWRITE_TEXT_RANGE, FontWeight> &p){
			layout->SetFontWeight(p.second, p.first);
	});
	SetColors();

	return true;
}

void Text::SetColors()
{
	std::for_each(
		colors.begin(), colors.end(),
		[this](std::pair<DWRITE_TEXT_RANGE, SolidBrush> &p){
			layout->SetDrawingEffect(p.second.Get(), p.first);
	});
}

Text::Text(const Text &t):
	layout(nullptr),
	brush(t.brush),
	wordwrapping(t.wordwrapping),
	align(t.align),
	p_align(t.p_align),
	fontsizes(t.fontsizes),
	underlines(t.underlines),
	italics(t.italics),
	strikethroughs(t.strikethroughs),
	colors(t.colors),
	modified(true)
{
}

Text::Text(Text &&t):
	layout(nullptr),
	brush(t.brush),
	wordwrapping(t.wordwrapping),
	align(t.align),
	p_align(t.p_align),
	fontsizes(std::move(t.fontsizes)),
	underlines(std::move(t.underlines)),
	italics(std::move(t.italics)),
	strikethroughs(std::move(t.strikethroughs)),
	colors(t.colors),
	modified(true)
{
}

Text::~Text()
{
	SafeRelease(layout);
}

void Text::SetFont(const ChaoticLib::Direct2D::Font &f)
{
	font = f;
}

void Text::SetFont(ChaoticLib::Direct2D::Font &&f)
{
	font = std::move(f);
}

void Text::SetText(const wchar_t *t)
{
	modified = true;
	text = t;
}

void Text::SetColor(Color c, int start /* =-1 */, int end /* =-1 */)
{
	if(start < 0 || end <= 0)
		brush.SetColor(c);
	else
		colors.push_back(std::make_pair(CreateRange(start, end), c));
}

void Text::SetWordWrapping(WordWrapping ww)
{
	wordwrapping = ww;

	if(layout != nullptr)
		layout->SetWordWrapping(ww);
}

void Text::SetAlign(Align a)
{
	 align = a;

	 if(layout != nullptr)
		 layout->SetTextAlignment(a);
}

void Text::SetParagraphAlign(ParagraphAlign pa)
{
	p_align = pa;

	 if(layout != nullptr)
		 layout->SetParagraphAlignment(pa);
}

void Text::SetUnderline(int start, int end, bool set /* =true */)
{
	if(start < 0 || end <= 0){
		underlines.clear();
		if(set)
			underlines.push_back(std::make_pair(CreateRange(0, -1), true));
	}else
		underlines.push_back(std::make_pair(CreateRange(start, end), set));
}

void Text::SetItalic(int start, int end, bool set /* =true */)
{
	if(start < 0 || end <= 0){
		italics.clear();
		if(set)
			italics.push_back(std::make_pair(CreateRange(0, -1), true));
	}else
		italics.push_back(std::make_pair(CreateRange(start, end), set));
}

void Text::SetStrikeThrough(int start, int end, bool set /* =true */)
{
	if(start < 0 || end <= 0){
		italics.clear();
		if(set)
			strikethroughs.push_back(std::make_pair(CreateRange(0, -1), true));
	}else
		strikethroughs.push_back(std::make_pair(CreateRange(start, end), set));
}

ChaoticLib::Direct2D::Font &Text::GetFont()
{
	return font;
}

const wchar_t *Text::GetText() const
{
	return text.c_str();
}

Text::Align Text::GetAlign() const
{
	return align;
}

Text::ParagraphAlign Text::GetParagraphAlign() const
{
	return p_align;
}

Text::WordWrapping Text::GetWordWrapping() const
{
	return wordwrapping;
}

Rect Text::GetDrawingRect()
{
	if(modified){
		SafeRelease(layout);
		if(!Create())
			throw std::runtime_error("could not create IDWriteTextLayout");
	}

	DWRITE_OVERHANG_METRICS dom;
	layout->GetOverhangMetrics(&dom);
	return Rect(
		Point(pos.x - dom.left, pos.y - dom.top),
		Size(size.width + dom.left + dom.right, size.height + dom.top + dom.bottom));
}

bool Text::CreateResource(const CreateStruct &cs)
{
	if(!brush.CreateResource(cs))
		return false;
	if(!std::all_of(
		colors.begin(), colors.end(),
		[&cs](std::pair<DWRITE_TEXT_RANGE, SolidBrush> &p){
			return p.second.CreateResource(cs);
	}))
		return false;

	if(layout == nullptr){
		if(!Create())
			return false;
	}else
		 SetColors();

	return true;
}

void Text::DestroyResource()
{
	SafeRelease(layout);
	brush.DestroyResource();
	std::for_each(
		colors.begin(), colors.end(),
		[](std::pair<DWRITE_TEXT_RANGE, SolidBrush> &p){
			p.second.DestroyResource();
	});
}

void Text::Draw(const PaintStruct &ps)
{
	if(modified){
		SafeRelease(layout);
		Create();
	}

	ps.target->DrawTextLayout(
		pos,
		layout,
		brush.Get());
}

IDWriteTextLayout *Text::Get()
{
	if(modified){
		DestroyResource();
		Create();
	}

	return layout;
}
