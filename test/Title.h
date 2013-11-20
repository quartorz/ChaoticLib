#pragma once

#include "Aliases.h"

template <class Window>
class Title: public Aliases::Scene<Window>{
	using Base = Aliases::Scene<Window>;

public:
	Title(Window *w): Base(w)
	{
	}
};
