#pragma once

#include "Aliases.h"

class Button: public Aliases::FlatButton{
public:
	Button()
	{
		SetColor(State::None, Aliases::Color(0, 0, 0, 50));
		SetColor(State::Hover, Aliases::Color(50, 50, 50, 20));
		SetColor(State::Push, Aliases::Color(0, 0, 0, 80));
		SetTextColor(State::None, Aliases::Color(255, 255, 255, 200));
		SetTextColor(State::Hover, Aliases::Color(255, 255, 255));
		SetTextColor(State::Push, Aliases::Color(255, 255, 255, 150));
	}
};
