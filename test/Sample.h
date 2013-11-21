#pragma once

#include "Aliases.h"
#include "ChaoticLib\Win32\Joystick.h"
#include "ChaoticLib\Win32\MessageLoop.h"
namespace WindowSystem = Aliases::WindowSystem;

#include "Title.h"
#include "Configurator.h"

class MainWindow:
	public WindowSystem::Window<
		MainWindow,
		WindowSystem::Resizable<MainWindow>,
		WindowSystem::Timer<MainWindow>,
		WindowSystem::Keyboard<MainWindow>,
		WindowSystem::Joystick<MainWindow, 16, 100>,
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
	};

private:
	Title<MainWindow> title;
	Configurator<MainWindow> config;

public:
	MainWindow():
		title(this),
		config(this)
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
