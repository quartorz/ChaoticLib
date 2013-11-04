#pragma once

#include "ChaoticLib\Win32\Window.h"
#include "ChaoticLib\Win32\Procs.h"
#include "ChaoticLib\Win32\Creator.h"
#include "ChaoticLib\Win32\MessageLoop.h"
#include "ChaoticLib\Direct2D\Painter.h"
#include "ChaoticLib\Direct2D\Traits.h"
#include "ChaoticLib\Direct2D\SolidBrush.h"
#include "ChaoticLib\Direct2D\Font.h"
#include "ChaoticLib\ResourceManager.h"

using namespace ChaoticLib;

class Window:
	public Win32::Window<
		Window,
		Win32::Movable<Window>,
		Win32::QuitOnClose<Window>,
		Win32::Timer<Window, 100>,
		Direct2D::Painter<Window>,
		ResourceManager<Window, Direct2D::Traits>
	>,
	public Win32::Creator<Window>
{
	// template <UINT ID>
	// using Timer = Win32::Timer<Window, ID>;
	template <UINT ID>
	struct Timer
	{
		typedef Win32::Timer<Window, ID> type;
	};

	Direct2D::SolidBrush *b;
	Direct2D::Font *font;

public:
	static const wchar_t *classname;

	bool Initialize()
	{
		b = CreateSolidBrush(Direct2D::Color(0, 0, 255));
		font = CreateFont();
		font->SetFontSize(40.f);
		Timer<100>::type::SetTimer(2000);

		return true;
	}

	void Uninitialize()
	{
		DeleteResource(b);
		DeleteResource(font);
	}

	void OnMove(int x, int y)
	{
		::OutputDebugStringW(L"move\n");
	}

	void Draw(const PaintStruct &ps)
	{
		Clear(ps, Direct2D::Color(1.f));

		int w;
		std::tie(w, std::ignore) = GetSize();
		DrawLine(ps, b, Direct2D::Point(0, 0), Direct2D::Point(w / 2, 100));
		DrawText(ps, font, b, L"Text", Direct2D::Rect(30.f, 60.f, 500.f, 600.f));
	}

	void OnTimer(unsigned id)
	{
		if(id == 100){
			::OutputDebugStringW(L"WM_TIMER\n");
			Timer<100>::type::KillTimer();
		}
	}
};

const wchar_t *Window::classname = L"Window";
