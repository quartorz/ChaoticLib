#pragma once

#include "ChaoticLib\Win32\Window.h"
#include "ChaoticLib\Win32\Procs.h"
#include "ChaoticLib\Win32\Creator.h"
#include "ChaoticLib\Win32\ObjectProcessor.h"
#include "ChaoticLib\Win32\MessageLoop.h"
#include "ChaoticLib\Direct2D\BaseTypes.h"
#include "ChaoticLib\Direct2D\Painter.h"
#include "ChaoticLib\Direct2D\Traits.h"
#include "ChaoticLib\Scene.h"
#include "ChaoticLib\SceneManager.h"

#include "MyUserDefinedObject.h"

using namespace ChaoticLib;

template <class Window>
class SceneTest: public Scene<Window, Direct2D::Traits>
{
	MyUserDefinedObject<Window> userdefined0, userdefined1;

public:
	SceneTest(Window *w): Scene(w), userdefined0(w), userdefined1(w)
	{
		RegisterObject(&userdefined0);
		RegisterObject(&userdefined1);
		userdefined0.SetRect(Direct2D::Rect(0, 0, 100, 200));
		userdefined1.SetRect(Direct2D::Rect(0, 0, 100, 200));
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
			o->SetSize(Direct2D::Size(100, 200));
		}
	}
};

class Window:
	public Win32::Window<
		Window,
		Win32::QuitOnClose<Window>,
		Win32::Resizable<Window>, // 無いとSceneManager::OnSizeが呼ばれない→何も表示されない
		Win32::Keyboard<Window>,
		Win32::ObjectProcessor<Window, Direct2D::Traits>,
		Win32::Timer<Window, 100>,
		Direct2D::Painter<Window>
	>,
	public SceneManager<Window, Direct2D::Traits>,
	public Win32::Creator<Window>
{
	SceneTest<Window> scene0, scene1;

public:
	static const wchar_t *classname;

	Window(): scene0(this), scene1(this)
	{
	}

	bool Initialize()
	{
		Win32::Timer<Window, 100>::SetTimer(16);
		AddScene(&scene0);
		AddScene(&scene1);
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

const wchar_t *Window::classname = L"Window";
