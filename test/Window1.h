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

#include "Aliases.h"

using namespace Aliases;

class MainWindow:
	public WindowSystem::Window<
		MainWindow,
		WindowSystem::Movable<MainWindow>,
		WindowSystem::QuitOnClose<MainWindow>,
		WindowSystem::Timer<MainWindow, 100>,
		Painter<MainWindow>
	>,
	public ResourceManager<MainWindow>,
	public WindowSystem::Creator<MainWindow>
{
	// template <UINT ID>
	// using Timer = Win32::Timer<MainWindow, ID>;
	template <UINT ID>
	struct Timer
	{
		typedef WindowSystem::Timer<MainWindow, ID> type;
	};

	SolidBrush *b;
	Aliases::Font *font;

public:
	static const wchar_t *classname;

	bool Initialize()
	{
		b = CreateSolidBrush(Color(0, 0, 255));
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
		Clear(ps, Color(1.f));

		int w;
		std::tie(w, std::ignore) = GetSize();
		DrawLine(ps, b, Line(Point(0, 0), Point(w / 2, 100)));
		DrawText(ps, font, b, L"Text", Rect(30.f, 60.f, 500.f, 600.f));
	}

	void OnTimer(unsigned id)
	{
		if(id == 100){
			::OutputDebugStringW(L"WM_TIMER\n");
			Timer<100>::type::KillTimer();
		}
	}
};

const wchar_t *MainWindow::classname = L"Window";
