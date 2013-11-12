#pragma once

#include "ChaoticLib\Win32\MessageLoop.h"

#include "Aliases.h"

#include <vector>
#include <utility>
#include <algorithm>
#include <random>
#include <ctime>
#include <string>

#include <iostream>

template <class Window>
class Title: public Aliases::Scene<Window>
{
	class Button: public Aliases::FlatButton
	{
		Title *title;

		Aliases::Polygon pol;
		Aliases::SolidBrush *brush;

	public:
		Button(Title *t): title(t), pol({Aliases::Point(), Aliases::Point(22.f, 13.f), Aliases::Point(0.f, 26.f)})
		{
			pol.SetPosition(Aliases::Point(10.f, 0.f));
			pol.Finalize();
			brush = title->CreateSolidBrush(Aliases::Color(255, 0, 0));
		}
		~Button()
		{
			title->DeleteResource(brush);
		}

		void OnPush() override
		{
			title->GetWindow()->ChangeScene(Window::Scene::Game);
		}

		void SetState(State s) override
		{
			Aliases::FlatButton::SetState(s);
			title->GetWindow()->Repaint();
		}

		void SetPosition(const Aliases::Point &p) override
		{
			this->Aliases::FlatButton::SetPosition(p);
		}

		bool IsColliding(const Aliases::Point &ap) override
		{
			if(ap.IsInside(pol))
				return false;
			return this->Aliases::FlatButton::IsColliding(ap);
		}

		void Draw(const Aliases::PaintStruct &ps) override
		{
			this->Aliases::FlatButton::Draw(ps);
			pol.Fill(ps, brush);
		}
	}button;

public:
	Title(Window *w): Scene(w), button(this)
	{
		this->RegisterObject(&button);
		button.SetSize(Aliases::Size(800, 600));
		button.SetText(L"Click to start");
		button.SetTextSize(30.f);
		button.SetColor(Button::State::None, Aliases::Color(255, 255, 255, 0));
		button.SetColor(Button::State::Hover, Aliases::Color(255, 255, 255, 0));
		button.SetColor(Button::State::Push, Aliases::Color(255, 255, 255, 0));
		button.SetTextColor(Button::State::None, Aliases::Color(100, 100, 100));
		button.SetTextColor(Button::State::Hover, Aliases::Color(128, 128, 128));
		button.SetTextColor(Button::State::Push, Aliases::Color());
	}

	virtual void Draw(const Aliases::PaintStruct &ps) override
	{
		this->GetWindow()->Clear(ps, Aliases::Color(0, 0, 0, 0));
		this->Aliases::Scene<Window>::Draw(ps);
	}
};

template <class Window>
class Game: public Aliases::Scene<Window>
{
	Aliases::SolidBrush *food_brush, *body_brush;
	std::pair<int, int> food;
	std::vector<std::pair<int, int>> body;
	std::mt19937 rand{static_cast<unsigned int>(std::time(nullptr))};

	enum Direction{
		Up,
		Right,
		Down,
		Left
	}direction = Up;

	bool turned = false;

	void TurnLeft()
	{
		direction = static_cast<Direction>(static_cast<int>(direction)-1);
		if(direction < 0)
			direction = static_cast<Direction>(3);
		turned = true;
	}
	void TurnRight()
	{
		direction = static_cast<Direction>((direction + 1) % 4);
		turned = true;
	}
	void PutFood()
	{
		food = {rand() % 80, rand() % 60};
	}

	void Func(int, unsigned, bool)
	{
		::MessageBoxW(nullptr, L"Func", L"Func", MB_OK);
	}

public:
	Game(Window *w): Scene(w)
	{
		food_brush = this->CreateSolidBrush(Aliases::Color(255, 0, 0));
		body_brush = this->CreateSolidBrush(Aliases::Color());

		this->AddKeyboardHandler([this](unsigned keycode, bool is_push){
			if(!turned && is_push){
				if(keycode == VK_LEFT){
					TurnLeft();
					turned = true;
				}else{
					TurnRight();
					turned = true;
				}
			}
		}, VK_LEFT, VK_RIGHT);

		this->AddKeyboardHandler(std::bind(&Game::Func, this, 0, std::placeholders::_1, std::placeholders::_2), L'Q');

		this->AddTimerHandler([this](unsigned){
			turned = false;

			std::copy_backward(body.begin(), body.end() - 1, body.end());

			auto &head = body[0];
			switch (direction){
			case Up:
				std::get<1>(head)--;
				break;
			case Right:
				std::get<0>(head)++;
				break;
			case Down:
				std::get<1>(head)++;
				break;
			case Left:
				std::get<0>(head)--;
				break;
			}
			if (std::get<0>(head) < 0
				|| std::get<1>(head) < 0
				|| std::get<0>(head) >= 80
				|| std::get<1>(head) >= 60
				|| std::find(body.begin() + 1, body.end(), body.front()) != body.end())
			{
				this->GetWindow()->WindowSystem::Timer<Window>::KillTimer(100);
				::MessageBoxW(this->GetWindow()->GetHwnd(), (L"記録:" + std::to_wstring(body.size())).c_str(), L"ゲームオーバー", MB_OK);
				this->GetWindow()->ChangeScene(Window::Scene::Title);
			}
			if (head == food){
				body.push_back(body.back());
				PutFood();
			}

			this->GetWindow()->Repaint();
		}, 100);
	}
	~Game()
	{
		this->DeleteResource(food_brush);
		this->DeleteResource(body_brush);
	}
	void Show() override
	{
		direction = Up;

		body.clear();
		body.push_back({40, 30});
		PutFood();

		this->GetWindow()->SetTimer(100, 100);
	}
	void Hide() override
	{
		this->GetWindow()->KillTimer(100);
	}
	void Draw(const Aliases::PaintStruct &ps) override
	{
		this->GetWindow()->Clear(ps, Aliases::Color(255, 255, 255, 100));
		Aliases::Circle(Aliases::Point(std::get<0>(food) * 10 + 5, std::get<1>(food) * 10 + 5), 5.f).Fill(ps, food_brush);
		for(auto &p: body){
			Aliases::Circle(Aliases::Point(std::get<0>(p) * 10 + 5, std::get<1>(p) * 10 + 5), 5.f).Fill(ps, body_brush);
		}
	}
};

namespace WindowSystem = Aliases::WindowSystem;

class MainWindow:
	public WindowSystem::Window <
		MainWindow,
		WindowSystem::QuitOnClose<MainWindow>,
		WindowSystem::Resizable<MainWindow>, // 無いとSceneManager::OnSizeが呼ばれない→何も表示されない
		WindowSystem::Keyboard<MainWindow>,
		Aliases::ObjectProcessor<MainWindow>,
		WindowSystem::Timer<MainWindow>,
		Aliases::Painter<MainWindow>
	>,
	public Aliases::SceneManager<MainWindow>,
	public WindowSystem::Creator<MainWindow>
{
public:
	enum class Scene{
		Title,
		Game,
	};

private:
	Title<MainWindow> title;
	Game<MainWindow> game;

public:
	static const wchar_t *classname;

	MainWindow(): title(this), game(this)
	{
	}

	bool Initialize()
	{
		this->AddScene(static_cast<int>(Scene::Title), &title);
		this->AddScene(static_cast<int>(Scene::Game), &game);

		auto hash = this->AddKeyboardHandler([](unsigned keycode, bool is_push){
			std::wcout << L"key: " << static_cast<wchar_t>(keycode) << L' ';
			if(is_push)
				std::wcout << L"push";
			else
				std::wcout << L"release";
			std::wcout << std::endl;
		}, L'A', L'B', std::make_tuple(L'A', L'J'));

		this->DeleteKeyboardHandler(hash, L'A', std::make_tuple(L'G', L'H'));

		this->SetTimer(200, 1000);
		auto h = this->AddTimerHandler([](unsigned id){std::wcout << L"id: " << id << std::endl;}, 100, 200);
		this->DeleteTimerHandler(h, 200);

		return true;
	}

	void Uninitialize()
	{
	}

	void ChangeScene(Scene scene)
	{
		this->SelectScene(static_cast<int>(scene));
	}
};

const wchar_t *MainWindow::classname = L"Window";
