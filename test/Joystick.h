#pragma once

#include "ChaoticLib\Win32\MessageLoop.h"
#include "Aliases.h"
#include "ChaoticLib\Win32\Joystick.h"

namespace WindowSystem = Aliases::WindowSystem;

#include <cstdio>

template <class Window>
class Scene: public Aliases::Scene<Window>
{
	using Base = Aliases::Scene<Window>;

	Aliases::Font *font;
	Aliases::SolidBrush *brush;

	DIJOYSTATE2 state = {};

public:
	Scene(Window *w): Base(w)
	{
		font = this->CreateFont();
		brush = this->CreateSolidBrush(Aliases::Color());

		font->SetFontName(L"Meiryo UI");

		this->AddTimerHandler([this](...){
			this->GetWindow()->Repaint();
		}, 100);

		this->AddJoystickHandler([this](DIJOYSTATE2 &js){
			state = js;
		});
	}
	~Scene()
	{
		this->DeleteResource(font);
		this->DeleteResource(brush);
	}

	void Draw(const Aliases::PaintStruct &ps) override
	{
		wchar_t text[2000];

		std::swprintf(text,
			L"Axis X: %ld\n"
			L"Axis Y: %ld\n"
			L"Axis Z: %ld\n"
			L"Rot X: %ld\n"
			L"Rot Y: %ld\n"
			L"Rot Z: %ld\n"
			L"Slider 0: %ld\n"
			L"Slider 1: %ld\n"
			L"POV 0: %ld\n"
			L"POV 1: %ld\n"
			L"POV 2: %ld\n"
			L"POV 3: %ld\n",
			state.lX,
			state.lY,
			state.lZ,
			state.lRx,
			state.lRy,
			state.lRz,
			state.rglSlider[0],
			state.rglSlider[1],
			state.rgdwPOV[0],
			state.rgdwPOV[1],
			state.rgdwPOV[2],
			state.rgdwPOV[3]);

		for(int i = 0; i < 128; ++i){
			wchar_t s[10];
			std::swprintf(s, L"%02X ", state.rgbButtons[i]);
			std::wcscat(text, s);
			if(i % 16 == 15)
				std::wcscat(text, L"\n");
		}

		int w;
		std::tie(w, std::ignore) = this->GetWindow()->GetSize();

		this->GetWindow()->DrawText(ps, font, brush, text, Aliases::Rect(0, 0, static_cast<float>(w), 1000));
	}
};

class MainWindow:
	public WindowSystem::Window<
		MainWindow,
		WindowSystem::Resizable<MainWindow>,
		WindowSystem::Timer<MainWindow>,
		WindowSystem::Joystick<MainWindow, 100, 100>,
		WindowSystem::QuitOnClose<MainWindow>,
		Aliases::ObjectProcessor<MainWindow>,
		Aliases::Painter<MainWindow>
	>,
	public WindowSystem::Creator<MainWindow>,
	public Aliases::SceneManager<MainWindow>
{
	Scene<MainWindow> scene;

public:
	static const wchar_t *classname;

	MainWindow(): scene(this)
	{
	}

	bool Initialize()
	{
		this->InitJoystickHandler();

		AddScene(0, &scene);
		return true;
	}

	void Uninitialize()
	{
	}
};

const wchar_t *MainWindow::classname = L"MainWindow";