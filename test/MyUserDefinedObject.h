#pragma once

#include "ChaoticLib\Direct2D\Object.h"
#include "ChaoticLib\Direct2D\Traits.h"
#include "ChaoticLib\FlatButton.h"
#include "ChaoticLib\Direct2D\UserDefinedObject.h"

#include <algorithm>
#include <functional>

template <class Window>
class MyUserDefinedObject: public ChaoticLib::Direct2D::UserDefinedObject<MyUserDefinedObject<Window>, Window>{
	typedef ChaoticLib::Direct2D::Object Object;
	typedef ChaoticLib::Direct2D::Point Point;
	typedef ChaoticLib::Direct2D::Size Size;
	typedef ChaoticLib::Direct2D::Color Color;
	typedef ChaoticLib::Direct2D::PaintStruct PaintStruct;
	typedef ChaoticLib::Direct2D::SolidBrush SolidBrush;
	typedef ChaoticLib::Direct2D::UserDefinedObject<MyUserDefinedObject<Window>, Window> super;

	class UserDefinedButton: public ChaoticLib::FlatButton<ChaoticLib::Direct2D::Traits>
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

	SolidBrush *brush;

	Point push_pos, origin;
	bool pushing;

public:
	MyUserDefinedObject(Window *w):
		super(w),
		pushing(false)
	{
		std::for_each(
			buttons, buttons + _countof(buttons),
			std::bind(
				&UserDefinedButton::SetSize,
				std::placeholders::_1,
				Size(100, 50)));

		Color colors[_countof(buttons)][3] = {
			{
				Color(255, 70, 70),
				Color(255, 80, 80),
				Color(240, 0, 0),
			},
			{
				Color(90, 90, 255),
				Color(100, 100, 255),
				Color(0, 0, 240),
			},
			{
				Color(70, 255, 70),
				Color(120, 255, 120),
				Color(0, 200, 0),
			},
		};
		Point pt = Point(10, 10);
		for(unsigned i = 0; i < _countof(buttons); ++i){
			buttons[i].SetPosition(pt);
			buttons[i].SetNumber(i);
			buttons[i].SetColor(UserDefinedButton::State::None, colors[i][0]);
			buttons[i].SetColor(UserDefinedButton::State::Hover, colors[i][1]);
			buttons[i].SetColor(UserDefinedButton::State::Push, colors[i][2]);
			buttons[i].SetText((L"Button " + std::to_wstring(static_cast<long long>(i))).c_str());
			this->RegisterObject(&buttons[i]);

			pt.y += 60.f;
		}

		SetPosition(Point());

		brush = this->CreateSolidBrush(Color());
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

	virtual bool IsColliding(const Point &ap) override
	{
		if(super::IsColliding(ap))
			return true;
		return ap.IsInside(this->GetRect());
	}

	virtual void OnLeftPress(const Point &ap, Object::HitTestStruct &hts) override
	{
		super::OnLeftPress(ap, hts);

		if(!super::IsColliding(ap)){
			pushing = true;
			hts.SetCursor(Object::HitTestStruct::Cursor::Hand);
			origin = this->GetPosition();
			push_pos = ap;
		}
	}

	virtual void OnMouseMove(const Point &ap, Object::HitTestStruct &hts) override
	{
		if(!pushing){
			super::OnMouseMove(ap, hts);
			if(!super::IsColliding(ap))
				hts.SetCursor(Object::HitTestStruct::Cursor::Hand);
		}else {
			hts.SetCursor(Object::HitTestStruct::Cursor::Hand);
			this->SetPosition(origin + ap - push_pos);
		}
	}

	virtual void OnLeftRelease(const Point &ap, Object::HitTestStruct &hts) override
	{
		pushing = false;
		super::OnLeftRelease(ap, hts);
	}

	virtual void Draw(const PaintStruct &ps) override
	{
		super::Draw(ps);
		this->GetRect().Draw(ps, *brush);
	}
};
