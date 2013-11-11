#pragma once

#include "ChaoticLib\Win32\MessageLoop.h"

#include "Aliases.h"

using namespace Aliases;
using namespace Aliases::WindowSystem;

template <class Window>
class SceneTest: public Scene<Window>
{
	class Button: public FlatButton
	{
		Scene *scene;
	public:
		Button(Scene *s): scene(s)
		{
		}
		virtual void OnPush() override
		{
		}
	}button;

public:
	SceneTest(Window *w): Scene(w), button(this)
	{
		RegisterObject(&button);
		button.SetSize(Size(200, 100));
		button.SetColor(Button::State::None, Color(255, 70, 70, 100));
		button.SetColor(Button::State::Hover, Color(255, 80, 80));
		button.SetColor(Button::State::Push, Color(240, 0, 0));
		button.SetTextColor(Button::State::None, Color(0, 0, 0, 128));
		button.SetText(L"Button");
	}
	~SceneTest()
	{
		UnregisterObject(&button);
	}

	virtual void Draw(const PaintStruct &ps) override
	{
		this->GetWindow()->Clear(ps, Color(255, 255, 255, 128));
		this->Scene::Draw(ps);
	}

	virtual void OnKeyDown(unsigned keycode) override
	{
		static int scene = 0;
		if(keycode == L' ')
			GetWindow()->SelectScene(scene = !scene);
	}
};

class MainWindow:
	public Window<
		MainWindow,
		QuitOnClose<MainWindow>,
		Resizable<MainWindow>, // ñ≥Ç¢Ç∆SceneManager::OnSizeÇ™åƒÇŒÇÍÇ»Ç¢Å®âΩÇ‡ï\é¶Ç≥ÇÍÇ»Ç¢
		Keyboard<MainWindow>,
		Aliases::ObjectProcessor<MainWindow>,
		Timer<MainWindow, 100>,
		Painter<MainWindow>
	>,
	public SceneManager<MainWindow>,
	public ChaoticLib::Win32::Creator<MainWindow>
{
	template <unsigned ID>
	using Timer = WindowSystem::Timer<MainWindow, ID>;

	SceneTest<MainWindow> scene0, scene1;

public:
	static const wchar_t *classname;

	MainWindow(): scene0(this), scene1(this)
	{
	}

	bool Initialize()
	{
		Timer<100>::SetTimer(16);
		AddScene(0, &scene0);
		AddScene(1, &scene1);
		return true;
	}

	void Uninitialize()
	{
	}

	void OnTimer(UINT id)
	{
		if(id == 100)
			Repaint();
	}
};

const wchar_t *MainWindow::classname = L"Window";
