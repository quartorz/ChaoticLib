#pragma once

#include "Aliases.h"

#include "KeyConfig.h"

#include <array>

template <class Window>
class Configurator: public Aliases::Scene<Window>{
	using Base = Aliases::Scene<Window>;

	class KeyAssignment: public Aliases::UserDefinedObject<KeyAssignment, Window>{
		using Base = Aliases::UserDefinedObject<KeyAssignment, Window>;

		class Key: public Aliases::FlatButton{
		public:
			void SetState(State s) override
			{
				this->Aliases::FlatButton::SetState(s);
			}
			void OnPush() override
			{
			}
			/*bool IsColliding(const Aliases::Point &) override
			{
				return false;
			}*/
		};

		Configurator *parent;

		std::wstring name_str;
		Aliases::Text name;
		Key key;
		int number;
		bool push = false, focus = false;

	public:
		KeyAssignment(Configurator *c, const wchar_t *n, int number):
			Base(c->GetWindow()),
			parent(c),
			number(number)
		{
			this->RegisterObject(&name);
			name.SetText(n);
			name.SetPosition(Aliases::Point(10, 10));
			name.SetSize(Aliases::Size(100, 40));
			name.SetAlign(Aliases::Text::Align::Right);
			name.SetParagraphAlign(Aliases::Text::ParagraphAlign::Center);
			name.GetFont().SetFontSize(20.f);

			using State = Key::State;

			this->RegisterObject(&key);
			key.SetColor(State::None, Aliases::Color(0, 0, 0, 50));
			key.SetColor(State::Hover, Aliases::Color(0, 0, 0, 20));
			key.SetColor(State::Push, Aliases::Color(0, 0, 0, 80));
			key.SetTextColor(State::None, Aliases::Color(255, 255, 255, 200));
			key.SetTextColor(State::Hover, Aliases::Color(255, 255, 255));
			key.SetTextColor(State::Push, Aliases::Color(255, 255, 255, 200));
			key.SetSize(Aliases::Size(150, 40));
			key.SetPosition(Aliases::Point(140, 10));
		}
		void SetName(const wchar_t *n)
		{
			name.SetText(n);
		}
		bool IsColliding(const Aliases::Point &p) override
		{
			return p.IsInside(this->GetRect());
		}
		void OnMouseMove(const Aliases::Point &p, Aliases::Object::HitTestStruct &hts) override
		{
			hts.SetCursor(Aliases::Object::HitTestStruct::Cursor::Hand);
			if(focus)
				return;
			if(IsColliding(p)){
				if(push)
					key.SetState(Key::State::Push);
				else
					key.SetState(Key::State::Hover);
			} else
				key.SetState(Key::State::Hover);
		}
		void OnLeftPress(const Aliases::Point &p, Aliases::Object::HitTestStruct &hts) override
		{
			push = true;
			key.SetState(Key::State::Push);
		}
		void OnLeftRelease(const Aliases::Point &p, Aliases::Object::HitTestStruct &hts) override
		{
			push = false;
			if(this->IsColliding(p)){
				focus = true;
				key.SetState(Key::State::Push);
				key.SetText(L"Push Key");
				parent->SelectAssignor(number);
			}
		}
		void OnMouseLeave(const Aliases::Object::HitTestStruct &hts) override
		{
			if(!focus)
				key.SetState(Key::State::None);
		}
		void OnLoseFocus(const Aliases::Object::HitTestStruct &hts) override
		{
			focus = false;
			key.SetState(Key::State::None);
			key.SetText(name_str.c_str());
			parent->SelectAssignor(-1);
		}
		void Draw(const Aliases::PaintStruct &ps) override
		{
			// deprecated
			ps.target->PushAxisAlignedClip(key.GetRect() + this->GetPosition(), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
			this->GetWindow()->Clear(ps, Aliases::Color(0, 0, 0, 0));
			ps.target->PopAxisAlignedClip();

			this->Base::Draw(ps);
		}
		void SetKeyAssign(const wchar_t *n)
		{
			name_str = n;
			if(!focus)
				key.SetText(n);
		}
	};

	void SelectAssignor(int n)
	{
		assignor = n;
	}

	KeyConfig config;
	std::vector<std::tuple<joystick_id, std::wstring>> joysticks;

	Aliases::Text *title, *text[2];

	std::vector<KeyAssignment> assignors;
	int assignor = -1;

	Aliases::Font *font;
	Aliases::SolidBrush *brush;
	Aliases::Text *js_name_text;
	std::wstring js_name;

public:
	Configurator(Window *w): Base(w), config(KeyConfigClass::GetInstance())
	{
		title = this->CreateText();
		text[0] = this->CreateText();
		text[1] = this->CreateText();
		font = this->CreateFont();
		brush = this->CreateSolidBrush(Aliases::Color());
		js_name_text = this->CreateText();

		title->SetText(L"Key Config");
		title->SetSize(Aliases::Size(800, 70));
		title->SetAlign(Aliases::Text::Align::Center);
		title->SetParagraphAlign(Aliases::Text::ParagraphAlign::Center);
		title->GetFont().SetFontSize(30.f);

		text[0]->SetText(L"Keyboard");
		text[0]->SetSize(Aliases::Size(400, 70));
		text[0]->SetPosition(Aliases::Point(0, 60));
		text[0]->SetAlign(Aliases::Text::Align::Center);
		text[0]->SetParagraphAlign(Aliases::Text::ParagraphAlign::Center);
		text[0]->GetFont().SetFontSize(25.f);

		text[1]->SetText(L"Joystick");
		text[1]->SetSize(Aliases::Size(400, 70));
		text[1]->SetPosition(Aliases::Point(400, 60));
		text[1]->SetAlign(Aliases::Text::Align::Center);
		text[1]->SetParagraphAlign(Aliases::Text::ParagraphAlign::Center);
		text[1]->GetFont().SetFontSize(25.f);

		js_name_text->SetPosition(Aliases::Point(480, 510));
		js_name_text->SetSize(Aliases::Size(310, 200));

		this->AddTimerHandler([this](unsigned id){
			this->GetWindow()->Repaint();
		}, 100);

		static const wchar_t *keynames[] ={
			L"Up",
			L"Down",
			L"Left",
			L"Right",
			L"A",
			L"B",
			L"Menu",
		};
		assignors.reserve(14);
		for(unsigned i = 0; i < 14; ++i){
			assignors.emplace_back(this, keynames[i % 7], i);
			this->RegisterObject(&assignors.back());
			assignors.back().SetSize(Aliases::Size(300, 60));
			if(i < 7){
				assignors.back().SetPosition(Aliases::Point(50, 120 + 50.f * i));
				assignors.back().SetKeyAssign(config->GetKeyboardConfig(static_cast<KeyConfigClass::Button>(i)));
			}else{
				assignors.back().SetPosition(Aliases::Point(450, 120 + 50.f * (i % 7)));
				assignors.back().SetKeyAssign(config->GetJoystickConfig(static_cast<KeyConfigClass::Button>(i - 7)));
			}
		}

		this->AddKeyboardHandler([this](unsigned keycode, bool push){
			if(push && 0 <= assignor && assignor < 7){
				auto str = config->SetKeyboardConfig(static_cast<KeyConfigClass::Button>(assignor), keycode);
				assignors[assignor].SetKeyAssign(str);
				assignors[assignor].OnLoseFocus(Aliases::Object::CreateHitTestStruct(this->GetWindow()));
				assignor = -1;
			}
		}, std::tuple<wchar_t, wchar_t>(0, 255));
	}
	~Configurator()
	{
		this->DeleteObject(title);
		this->DeleteObject(text[0]);
		this->DeleteObject(text[1]);
		this->DeleteResource(font);
		this->DeleteResource(brush);
	}

	void Show() override
	{

	}

	void OnGetJoystickState(const joystick_id &id, const DIJOYSTATE2 &js) override
	{
		if(assignor >= 7){
			auto str = config->SetJoystickConfig(static_cast<KeyConfigClass::Button>(assignor - 7), id, js);
			if(std::wcsncmp(str, L"None", 5)){
				assignors[assignor].SetKeyAssign(str);
				assignors[assignor].OnLoseFocus(Aliases::Object::CreateHitTestStruct(this->GetWindow()));
				assignor = -1;

				auto it = std::find_if(joysticks.begin(), joysticks.end(), [&id](std::tuple<joystick_id, std::wstring> &t){return std::get<0>(t) == id; });
				if(js_name != std::get<1>(*it)){
					js_name = std::get<1>(*it);
					js_name_text->SetText(js_name.c_str());
				}
			}
		}
	}
	void OnReloadJoysticks(const std::vector<std::tuple<joystick_id, std::wstring>> &jss) override
	{
		joysticks = jss;
	}

	void Draw(const Aliases::PaintStruct &ps) override
	{
		ps.target->Clear(Aliases::Color(255, 255, 255, 200)); // deprecated

		this->GetWindow()->DrawText(ps, font, brush, L"Joystick name: ", Aliases::Rect(450, 480, 800, 600));

		this->Base::Draw(ps);
	}
};
