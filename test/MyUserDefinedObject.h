#pragma once

#include "ChaoticLib\Direct2D\Object.h"
#include "ChaoticLib\Direct2D\Traits.h"
#include "ChaoticLib\FlatButton.h"
#include "ChaoticLib\Direct2D\UserDefinedObject.h"

#include <algorithm>
#include <functional>

#include "Aliases.h"

template <class Window>
class MyUserDefinedObject: public Aliases::UserDefinedObject<MyUserDefinedObject<Window>, Window>{
	typedef Aliases::UserDefinedObject<MyUserDefinedObject<Window>, Window> Base;

	class UserDefinedButton: public FlatButton
	{
		int number;

	public:
		void SetNumber(int n)
		{
			number = n;
		}
		virtual void OnPush() override
		{
			::OutputDebugStringW((L"Button " + std::to_wstring(static_cast<long long>(number)) + L"\n").c_str());
		}
	}buttons[3];

	Aliases::SolidBrush *brush;

	Aliases::Point push_pos, origin;
	bool pushing;

public:
	MyUserDefinedObject(Window *w):
		Base(w),
		pushing(false)
	{

		Aliases::Color colors[_countof(buttons)][3] ={
			{
				Aliases::Color(255, 70, 70),
				Aliases::Color(255, 80, 80),
				Aliases::Color(240, 0, 0),
			},
			{
				Aliases::Color(90, 90, 255),
				Aliases::Color(100, 100, 255),
				Aliases::Color(0, 0, 240),
			},
			{
				Aliases::Color(70, 255, 70),
				Aliases::Color(120, 255, 120),
				Aliases::Color(0, 200, 0),
			},
		};
		Aliases::Point pt = Aliases::Point(10, 10);
		for(unsigned i = 0; i < _countof(buttons); ++i){
			buttons[i].SetPosition(pt);
			buttons[i].SetSize(Aliases::Size(100, 50));
			buttons[i].SetNumber(i);
			buttons[i].SetColor(UserDefinedButton::State::None, colors[i][0]);
			buttons[i].SetColor(UserDefinedButton::State::Hover, colors[i][1]);
			buttons[i].SetColor(UserDefinedButton::State::Push, colors[i][2]);
			buttons[i].SetText((L"Button " + std::to_wstring(static_cast<long long>(i))).c_str());
			this->RegisterObject(&buttons[i]);

			pt.y += 60.f;
		}

		SetPosition(Aliases::Point());

		brush = this->CreateSolidBrush(Aliases::Color());
	}
	~MyUserDefinedObject()
	{
		std::for_each(
			buttons, buttons + _countof(buttons),
			[this](UserDefinedButton &button){
				UnregisterObject(&button);
		});
		this->DeleteResource(brush);
	}

	virtual bool IsColliding(const Aliases::Point &ap) override
	{
		if(Base::IsColliding(ap))
			return true;
		return ap.IsInside(this->GetRect());
	}

	virtual void OnLeftPress(const Aliases::Point &ap, Aliases::Object::HitTestStruct &hts) override
	{
		Base::OnLeftPress(ap, hts);

		if(!Base::IsColliding(ap)){
			pushing = true;
			hts.SetCursor(Aliases::Object::HitTestStruct::Cursor::Hand);
			origin = this->GetPosition();
			push_pos = ap;
		}
	}

	virtual void OnMouseMove(const Aliases::Point &ap, Aliases::Object::HitTestStruct &hts) override
	{
		if(!pushing){
			Base::OnMouseMove(ap, hts);
			if(!Base::IsColliding(ap))
				hts.SetCursor(Aliases::Object::HitTestStruct::Cursor::Hand);
		}else {
			hts.SetCursor(Aliases::Object::HitTestStruct::Cursor::Hand);
			this->SetPosition(origin + ap - push_pos);
		}
	}

	virtual void OnLeftRelease(const Aliases::Point &ap, Aliases::Object::HitTestStruct &hts) override
	{
		pushing = false;
		Base::OnLeftRelease(ap, hts);
	}

	virtual void Draw(const Aliases::PaintStruct &ps) override
	{
		Base::Draw(ps);
		this->GetRect().Draw(ps, *brush);
	}
};
