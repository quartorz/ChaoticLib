#pragma once

#include "Aliases.h"

#include "KeyConfig.h"
#include "Function.h"

template <class Window>
class Shooting: public Aliases::Scene<Window>{
	using Base = Aliases::Scene<Window>;

	class Player: public Aliases::UserDefinedObject<Player, Window>{
		using Base = Aliases::UserDefinedObject<Player, Window>;

		Aliases::Image image;

	public:
		Player(Window *w): Base(w)
		{
			this->RegisterObject(&image);
			image.SetFileName((GetDirectory() + L"Player.png").c_str());
			image.SetSize(Aliases::Size(50, 50));
			image.SetInterpolationMode(Aliases::Image::InterpolationMode::NearestNeighbor);

			this->SetSize(Aliases::Size(50, 50));
		}
		~Player()
		{
			this->UnregisterObject(&image);
		}
	};

	KeyConfig *config;

	Player player;

public:
	Shooting(Window *w): Base(w), config(KeyConfig::GetInstance()), player(w)
	{
		this->RegisterObject(&player);

		this->AddKeyboardHandler([this](unsigned keycode, bool push){
			config->SetKeyboardState(keycode, push);
		}, std::tuple<wchar_t, wchar_t>(0, 255));

		this->AddTimerHandler([this](unsigned){
			auto state = config->GetState();
			float velocity = 1.f;
			if(state.a){
				velocity = 2.f;
			}
			if(state.up){
				player.SetPosition(player.GetPosition() - Aliases::Point(0, velocity));
			}
			if(state.down){
				player.SetPosition(player.GetPosition() + Aliases::Point(0, velocity));
			}
			if(state.left){
				player.SetPosition(player.GetPosition() - Aliases::Point(velocity, 0));
			}
			if(state.right){
				player.SetPosition(player.GetPosition() + Aliases::Point(velocity, 0));
			}
			this->GetWindow()->Repaint();
		}, 100);
	}
	~Shooting()
	{
		this->UnregisterObject(&player);
	}
	void OnGetJoystickState(const joystick_id &id, const DIJOYSTATE2 &js) override
	{
		config->SetJoystickState(id, js);
	}
};
