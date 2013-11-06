#pragma once

#include "ChaoticLib\Win32\MessageLoop.h"

#include "Aliases.h"
#include "MyUserDefinedObject.h"

using namespace Aliases;
using namespace Aliases::WindowSystem;

template <class Window>
class SceneTest: public Scene<Window>
{
	MyUserDefinedObject<Window> userdefined0, userdefined1;

public:
	SceneTest(Window *w): Scene(w), userdefined0(w), userdefined1(w)
	{
		RegisterObject(&userdefined0);
		RegisterObject(&userdefined1);
		userdefined0.SetRect(Rect(0, 0, 100, 200));
		userdefined1.SetRect(Rect(0, 0, 100, 200));
	}
	~SceneTest()
	{
		UnregisterObject(&userdefined0);
		UnregisterObject(&userdefined1);
	}

	virtual void OnKeyDown(unsigned keycode) override
	{
		static int scene = 0;
		if(keycode == L' ')
			GetWindow()->SelectScene(scene = !scene);
		else if(keycode == L'A'){
			auto o = new MyUserDefinedObject<Window>(GetWindow());
			RegisterObject(o);
			o->SetSize(Size(100, 200));
		}
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
