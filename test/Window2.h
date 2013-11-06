#pragma once

#include "ChaoticLib\Win32\MessageLoop.h"

#include "Aliases.h"

using namespace Aliases;

class MainWindow:
	public WindowSystem::Window<
		MainWindow,
		WindowSystem::Movable<MainWindow>,
		WindowSystem::QuitOnClose<MainWindow>,
		WindowSystem::Timer<MainWindow, 100>,
		WindowSystem::Timer<MainWindow, 200>,
		WindowSystem::Keyboard<MainWindow>,
		Aliases::ObjectProcessor<MainWindow>,
		Painter<MainWindow>
	>,
	public Aliases::Manager<MainWindow>,
	public WindowSystem::Creator<MainWindow>
{
#ifndef DONT_USE_TEMPLATE_ALIASES
	template <UINT ID>
	using Timer = WindowSystem::Timer<MainWindow, ID>;
#else
	template <UINT ID>
	struct Timer
	{
		typedef Win32::Timer<MainWindow, ID> type;
	};
#endif

	SolidBrush *brush;
	Aliases::Font *font;
	Image *image;
	Text *text;

	class Button: public FlatButton
	{
	public:
		virtual void OnPush() override
		{
			::OutputDebugStringW(L"push\n");
		}
	}button[2];

public:
	static const wchar_t *classname;

	MainWindow()
	{
	}

	bool Initialize()
	{
		brush = CreateSolidBrush(Color(0, 0, 255));
		font = CreateFont();
		font->SetFontSize(40.f);
		Timer<100>::SetTimer(2000);
		Timer<200>::SetTimer(30);

		image = CreateImage();
		image->SetFileName(L"..\\Image.png");
		image->SetRect(Rect(10, 300, 600, 600));

		text = CreateText();
		text->SetText(L"Test\naaa\nbbb\n‚ ‚ ‚ ");
		text->SetWordWrapping(Text::WordWrapping::NoWrap);

		RegisterObject(&button[0]);
		button[0].SetRect(Rect(Point(200, 40), Size(300, 100)));
		button[0].SetText(L"Button1");
		button[0].SetColor(Button::State::None, Color(255, 70, 70));
		button[0].SetColor(Button::State::Hover, Color(255, 80,  80));
		button[0].SetColor(Button::State::Push, Color(240, 0, 0));
		button[0].SetTextColor(Button::State::None, Color(255, 255, 255));
		button[0].SetTextColor(Button::State::Hover, Color(255, 255, 255));
		button[0].SetTextColor(Button::State::Push, Color(255, 255, 255));
		button[0].SetTextSize(50.f);

		RegisterObject(&button[1]);
		button[1].SetRect(Rect(Point(350, 70), Size(300, 100)));
		button[1].SetText(L"Button2");
		button[1].SetColor(Button::State::None, Color(90, 90, 255, 128));
		button[1].SetColor(Button::State::Hover, Color(100, 100, 255, 200));
		button[1].SetColor(Button::State::Push, Color(0, 0, 240, 200));

		return true;
	}

	void Uninitialize()
	{
		DeleteResource(brush);
		DeleteResource(font);
		DeleteObject(image);
		DeleteObject(text);
		UnregisterObject(&button[0]);
		UnregisterObject(&button[1]);
	}

	void OnMove(int x, int y)
	{
		::OutputDebugStringW(L"move\n");
	}

	void Draw(const Aliases::PaintStruct &ps)
	{
		Clear(ps, Color(1.f, 1.f, 1.f));

		int w;
		std::tie(w, std::ignore) = GetSize();
		DrawLine(ps, brush, Line(Point(0, 0), Point(w / 2, 100)));
		DrawText(ps, font, brush, L"Text", Rect(30.f, 60.f, 500.f, 600.f));

		this->Aliases::Manager<MainWindow>::Draw(ps);
	}

	void OnTimer(unsigned id)
	{
		if(id == 100){
			::OutputDebugStringW(L"WM_TIMER\n");
			Timer<100>::KillTimer();
		}else if(id == 200){
			Repaint();
		}
	}

	void OnKeyDown(wchar_t keycode)
	{
		switch(keycode){
		case L'A':
			image->SetDrawingMode(Image::DrawingMode::Stretch);
			break;
		case L'B':
			image->SetDrawingMode(Image::DrawingMode::Squeeze);
			break;
		case L'C':
			image->SetDrawingMode(Image::DrawingMode::Trim);
			break;
		case L'D':
			image->SetInterpolationMode(Image::InterpolationMode::NearestNeighbor);
			break;
		case L'E':
			image->SetInterpolationMode(Image::InterpolationMode::Linear);
			break;
		}
		this->Repaint();
	}

	void OnKeyUp(unsigned keycode)
	{
	}
};

const wchar_t *MainWindow::classname = L"Window";
