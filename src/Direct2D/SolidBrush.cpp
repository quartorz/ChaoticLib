#include "SolidBrush.h"
#include "../Macro.h"

using namespace ChaoticLib::Direct2D;

void SolidBrush::SetColor(const Color &c)
{
	color = c;

	if(brush != nullptr){
		static_cast<ID2D1SolidColorBrush*>(brush)->SetColor(color);
	}
}

bool SolidBrush::CreateResource(const CreateStruct &cs)
{
	if(brush != nullptr)
		return true;
	return SUCCEEDED(cs.target->CreateSolidColorBrush(
		color,
		reinterpret_cast<ID2D1SolidColorBrush**>(&brush)));
}

void SolidBrush::DestroyResource()
{
	SafeRelease(brush);
}
