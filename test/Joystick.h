#pragma once

#include "ChaoticLib\Win32\MessageLoop.h"
#include "Aliases.h"
#include "ChaoticLib\Win32\Joystick.h"
#include "ChaoticLib\Win32\Internet.h"

namespace WindowSystem = Aliases::WindowSystem;

#include <cstdio>
#include <chrono>
#include <numeric>

template <class Window>
class Scene: public Aliases::Scene<Window>
{
	class Button: public Aliases::FlatButton{
		Scene *scene;
		unsigned number;

		void SetState(State s) override
		{
			this->Aliases::FlatButton::SetState(s);
			scene->GetWindow()->Repaint();
		}

	public:
		Button(Scene *s, unsigned n): scene(s), number(n)
		{
			this->SetColor(State::None, Aliases::Color(0, 0, 0, 50));
			this->SetColor(State::Hover, Aliases::Color(0, 0, 0, 20));
			this->SetColor(State::Push, Aliases::Color(0, 0, 0, 80));
			this->SetTextColor(State::None, Aliases::Color(255, 255, 255, 200));
			this->SetTextColor(State::Hover, Aliases::Color(255, 255, 255));
			this->SetTextColor(State::Push, Aliases::Color(255, 255, 255, 200));
			this->SetText((L"Joystick " + std::to_wstring(n + 1)).c_str());
			this->SetTextSize(20.f);
			this->SetSize(Aliases::Size(120, 50));
		}

		void OnPush() override
		{
			scene->SelectJoystick(number);
		}
	};

	using Base = Aliases::Scene<Window>;

	Aliases::Font *font;
	Aliases::SolidBrush *brush;

	WindowSystem::Internet internet;

	std::vector<Button> buttons;
	std::vector<joystick_id> js_ids;
	unsigned sel;
	DIJOYSTATE2 state = {};

	void SelectJoystick(unsigned n)
	{
		if(js_ids.size() == 0)
			return;

		this->UnregisterObject(&buttons[sel]);
		buttons[sel] = Button(this, sel);
		buttons[sel].SetPosition(Aliases::Point(670, 10 + 60.f * sel));
		this->RegisterObject(&buttons[sel]);
		buttons[n].SetColor(Button::State::None, Aliases::Color(0, 0, 0, 128));
		buttons[n].SetTextColor(Button::State::None, Aliases::Color(255, 255, 255, 128));
		sel = n;
	}

public:
	Scene(Window *w): Base(w), sel(0)
	{
		font = this->CreateFont();
		brush = this->CreateSolidBrush(Aliases::Color());

		font->SetFontName(L"Meiryo UI");

		this->AddTimerHandler([this](...){
			this->GetWindow()->Repaint();
		}, 100);

		internet.SetGETHandler([](void *buf, std::size_t len)->bool{
			::MessageBoxA(nullptr, static_cast<LPCSTR>(buf), "", MB_OK);
			return false;
		});
		internet.SetTimeout(std::chrono::milliseconds(1000));
		// internet.GET(L"http://www.google.co.jp/");
	}
	~Scene()
	{
		this->DeleteResource(font);
		this->DeleteResource(brush);

		for(auto &button : buttons){
			this->UnregisterObject(&button);
		}
	}

	void OnGetJoystickState(joystick_id &id, DIJOYSTATE2 &js) override
	{
		if(js_ids[sel] == id)
			state = js;
	}
	void OnReloadJoystick(const std::vector<joystick_id> &ids) override
	{
		sel = std::min(sel, ids.size() - 1);

		js_ids.clear();
		js_ids.assign(ids.begin(), ids.end());

		for(auto &button: buttons){
			this->UnregisterObject(&button);
		}

		buttons.clear();
		buttons.reserve(ids.size());
		unsigned c = 0;
		for(auto &id: ids){
			buttons.emplace_back(this, c);
			buttons.back().SetPosition(Aliases::Point(670, 10 + 60.f * c));
			this->RegisterObject(&buttons.back());
			c++;
		}

		SelectJoystick(sel);
	}

	void Draw(const Aliases::PaintStruct &ps) override
	{
		wchar_t text[2000];

		joystick_id &id = js_ids.size() > 0? js_ids[sel]: joystick_id();

		std::swprintf(text, 2000,
			L"Instance ID: %08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X\n"
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
			id.Data1,
			id.Data2,
			id.Data3,
			id.Data4[0],
			id.Data4[1],
			id.Data4[2],
			id.Data4[3],
			id.Data4[4],
			id.Data4[5],
			id.Data4[6],
			id.Data4[7],
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
			std::swprintf(s, 10, L"%02X ", state.rgbButtons[i]);
			::wcscat_s(text, s);
			if(i % 16 == 15)
				::wcscat_s(text, L"\n");
		}

		int w;
		std::tie(w, std::ignore) = this->GetWindow()->GetSize();

		this->GetWindow()->Clear(ps, Aliases::Color(255, 255, 255, 200));
		this->GetWindow()->DrawText(ps, font, brush, text, Aliases::Rect(0, 0, static_cast<float>(w), 1000));

		for(auto &button: buttons){
			ps.target->PushAxisAlignedClip(button.GetRect(), D2D1_ANTIALIAS_MODE_ALIASED);
			this->GetWindow()->Clear(ps, Aliases::Color(0, 0, 0, 0));
			ps.target->PopAxisAlignedClip();
		}
		this->Base::Draw(ps);
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
		AddScene(0, &scene);
		return true;
	}

	void Uninitialize()
	{
	}
};

const wchar_t *MainWindow::classname = L"MainWindow";
