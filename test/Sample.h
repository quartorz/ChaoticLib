#pragma once

#include "Aliases.h"
#include "ChaoticLib\Win32\Joystick.h"
#include "ChaoticLib\Win32\MessageLoop.h"
namespace WindowSystem = Aliases::WindowSystem;

class MainWindow:
	public WindowSystem::Window<
		MainWindow,
		WindowSystem::Resizable<MainWindow>,
		WindowSystem::Timer<MainWindow>,
		WindowSystem::Joystick<MainWindow, 16, 100>,
		WindowSystem::QuitOnClose<MainWindow>,
		Aliases::ObjectProcessor<MainWindow>,
		Aliases::Painter<MainWindow>
	>,
	public WindowSystem::Creator<MainWindow>,
	public Aliases::SceneManager<MainWindow>
{
public:
	static const wchar_t *GetClassName()
	{
		return L"MainWindow";
	}

	bool Initialize()
	{
		return true;
	}

	void Uninitialize()
	{
	}
};
