#pragma once

#include "ChaoticLib\Win32\Window.h"
#include "ChaoticLib\Win32\Procs.h"
#include "ChaoticLib\Win32\Creator.h"
#include "ChaoticLib\Win32\MessageLoop.h"
#include "ChaoticLib\Win32\ObjectProcessor.h"
#include "ChaoticLib\Direct2D\Painter.h"
#include "ChaoticLib\Direct2D\Traits.h"
#include "ChaoticLib\Direct2D\SolidBrush.h"
#include "ChaoticLib\Direct2D\Font.h"
#include "ChaoticLib\Direct2D\Image.h"
#include "ChaoticLib\FlatButton.h"
#include "ChaoticLib\Manager.h"

#include "MyUserDefinedObject.h"

using namespace ChaoticLib;

class Window:
	public Win32::Window<
		Window,
		Win32::Movable<Window>,
		Win32::QuitOnClose<Window>,
		Win32::Timer<Window, 100>,
		Win32::Timer<Window, 200>,
		Win32::Keyboard<Window>,
		Win32::ObjectProcessor<Window, Direct2D::Traits>,
		Direct2D::Painter<Window>
	>,
	public Manager<Window, Direct2D::Traits>,
	public Win32::Creator<Window>
{
	// template <UINT ID>
	// using Timer = Win32::Timer<Window, ID>;
	template <UINT ID>
	struct Timer
	{
		typedef Win32::Timer<Window, ID> type;
	};

	Direct2D::SolidBrush *brush;
	Direct2D::Font *font;
	Direct2D::Image *image;
	Direct2D::Text *text;

	class Button: public FlatButton<Direct2D::Traits>
	{
	public:
		virtual void OnPush() override
		{
			::OutputDebugStringW(L"push\n");
		}
	}button[2];

	MyUserDefinedObject<Window> userdefined;

public:
	static const wchar_t *classname;

	Window(): userdefined(this)
	{
	}

	bool Initialize()
	{
		brush = CreateSolidBrush(Direct2D::Color(0, 0, 255));
		font = CreateFont();
		font->SetFontSize(40.f);
		Timer<100>::type::SetTimer(2000);
		Timer<200>::type::SetTimer(30);

		image = CreateImage();
		image->SetFileName(L"..\\Image.png");
		image->SetRect(Direct2D::Rect(10, 300, 600, 600));

		text = CreateText();
		text->SetText(L"Test\naaa\nbbb\n‚ ‚ ‚ ");
		text->SetWordWrapping(Direct2D::Text::WordWrapping::NoWrap);

		RegisterObject(&button[0]);
		button[0].SetRect(Direct2D::Rect(Direct2D::Point(200, 40), Direct2D::Size(300, 100)));
		button[0].SetText(L"Button1");
		button[0].SetColor(Button::State::None, Direct2D::Color(255, 70, 70));
		button[0].SetColor(Button::State::Hover, Direct2D::Color(255, 80,  80));
		button[0].SetColor(Button::State::Push, Direct2D::Color(240, 0, 0));
		button[0].SetTextColor(Button::State::None, Direct2D::Color(255, 255, 255));
		button[0].SetTextColor(Button::State::Hover, Direct2D::Color(255, 255, 255));
		button[0].SetTextColor(Button::State::Push, Direct2D::Color(255, 255, 255));
		button[0].SetTextSize(50.f);

		RegisterObject(&button[1]);
		button[1].SetRect(Direct2D::Rect(Direct2D::Point(350, 70), Direct2D::Size(300, 100)));
		button[1].SetText(L"Button2");
		button[1].SetColor(Button::State::None, Direct2D::Color(90, 90, 255, 128));
		button[1].SetColor(Button::State::Hover, Direct2D::Color(100, 100, 255, 200));
		button[1].SetColor(Button::State::Push, Direct2D::Color(0, 0, 240, 200));

		RegisterObject(&userdefined);
		userdefined.SetSize(Direct2D::Size(80, 150));

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
		UnregisterObject(&userdefined);
	}

	void OnMove(int x, int y)
	{
		::OutputDebugStringW(L"move\n");
	}

	void Draw(const PaintStruct &ps)
	{
		Clear(ps, Direct2D::Color(1.f, 1.f, 1.f));

		int w;
		std::tie(w, std::ignore) = GetSize();
		DrawLine(ps, brush, Direct2D::Point(0, 0), Direct2D::Point(w / 2, 100));
		DrawText(ps, font, brush, L"Text", Direct2D::Rect(30.f, 60.f, 500.f, 600.f));

		Manager<Window, Direct2D::Traits>::Draw(ps);
	}

	void OnTimer(unsigned id)
	{
		if(id == 100){
			::OutputDebugStringW(L"WM_TIMER\n");
			Timer<100>::type::KillTimer();
		}else if(id == 200){
			Repaint();
		}
	}

	void OnKeyDown(wchar_t keycode)
	{
		switch(keycode){
		case L'A':
			image->SetDrawingMode(Direct2D::Image::DrawingMode::Stretch);
			break;
		case L'B':
			image->SetDrawingMode(Direct2D::Image::DrawingMode::Squeeze);
			break;
		case L'C':
			image->SetDrawingMode(Direct2D::Image::DrawingMode::Trim);
			break;
		case L'D':
			image->SetInterpolationMode(Direct2D::Image::InterpolationMode::NearestNeighbor);
			break;
		case L'E':
			image->SetInterpolationMode(Direct2D::Image::InterpolationMode::Linear);
			break;
		}
		this->Repaint();
	}

	void OnKeyUp(unsigned keycode)
	{
	}
};

const wchar_t *Window::classname = L"Window";
