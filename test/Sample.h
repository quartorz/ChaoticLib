#pragma once

#include "Aliases.h"
#include "ChaoticLib\Win32\Joystick.h"
#include "ChaoticLib\Win32\MessageLoop.h"
namespace WindowSystem = Aliases::WindowSystem;

#include "Title.h"
#include "Configurator.h"
#include "Shooting.h"
#include "Barrage.h"

class MainWindow:
	public WindowSystem::Window<
		MainWindow,
		WindowSystem::Resizable<MainWindow>,
		WindowSystem::Timer<MainWindow>,
		WindowSystem::Keyboard<MainWindow, true>,
		WindowSystem::Joystick<MainWindow, 100>,
		WindowSystem::QuitOnClose<MainWindow>,
		Aliases::ObjectProcessor<MainWindow>,
		Aliases::Painter<MainWindow>
	>,
	public WindowSystem::Creator<MainWindow>,
	public Aliases::SceneManager<MainWindow>
{
public:
	enum class Scene: int{
		Title,
		Config,
		Shooting,
	};

private:
	Title<MainWindow> title;
	Configurator<MainWindow> config;
	Shooting::Scene<MainWindow> shooting;
	Barrage::Scene<MainWindow> barrage;

public:
	MainWindow():
		title(this),
		config(this),
		shooting(this),
		barrage(this)
	{
	}

	static const wchar_t *GetClassName()
	{
		return L"MainWindow";
	}

	bool Initialize()
	{
		this->AddScene(Scene::Title, &title);
		this->AddScene(Scene::Config, &config);
		this->AddScene(Scene::Shooting, &shooting);

		SelectScene(Scene::Title);

		return true;
	}

	void Uninitialize()
	{
	}

	void AddScene(Scene n, Aliases::Scene<MainWindow> *scene)
	{
		this->Aliases::SceneManager<MainWindow>::AddScene(static_cast<int>(n), scene);
	}
	void SelectScene(Scene n)
	{
		this->Aliases::SceneManager<MainWindow>::SelectScene(static_cast<int>(n));
	}
};
