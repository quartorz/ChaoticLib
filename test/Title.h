#pragma once

#include "Aliases.h"

#include "KeyConfig.h"
#include "Button.h"

template <class Window>
class Title: public Aliases::Scene<Window>{
	using Base = Aliases::Scene<Window>;

	class Button: public ::Button{
		using Base = ::Button;

		Title *title;
		int number;

	public:
		Button(Title *t, int n): title(t), number(n)
		{
		}
		void OnPush() override
		{
			title->GetWindow()->SelectScene(static_cast<Window::Scene>(number));
		}
	};

	Button button;

public:
	Title(Window *w): Base(w), button(this, 1)
	{
		this->RegisterObject(&button);

		button.SetSize(Aliases::Size(150, 50));
		button.SetText(L"Key Config");

		this->AddTimerHandler([this](unsigned){
			this->GetWindow()->Repaint();
		}, 100);
	}
	~Title()
	{
		this->UnregisterObject(&button);
	}
	void Draw(const Aliases::PaintStruct &ps) override
	{
		// deprecated
		ps.target->PushAxisAlignedClip(button.GetRect(), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
		ps.target->Clear(Aliases::Color(0, 0, 0, 0));
		ps.target->PopAxisAlignedClip();

		this->Base::Draw(ps);
	}
};
