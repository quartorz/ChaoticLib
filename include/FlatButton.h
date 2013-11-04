#pragma once

#include "Button.h"

#include <algorithm>
#include <functional>

namespace ChaoticLib{

	template <class Traits>
	class FlatButton: public Button<Traits>
	{
		typename Traits::SolidBrush brush[3];
		typename Traits::Text text;
		typename Traits::Color color[3];

	public:
		FlatButton()
		{
			text.SetAlign(Traits::Text::Align::Center);
			text.SetParagraphAlign(Traits::Text::ParagraphAlign::Center);
		}

		void SetText(const wchar_t *t)
		{
			text.SetText(t);
		}

		void SetTextColor(State s, typename Traits::Color c)
		{
			color[s] = c;
		}

		void SetColor(State s, typename Traits::Color c)
		{
			brush[s].SetColor(c);
		}

		virtual void SetPosition(const typename Traits::Point &p) override
		{
			Button::SetPosition(p);
			text.SetPosition(p);
		}

		virtual void SetSize(const typename Traits::Size &s) override
		{
			Button::SetSize(s);
			text.SetSize(s);
		}

		bool CreateResource(const typename Traits::Resource::CreateStruct &cs) override
		{
			return text.CreateResource(cs)
				&& std::all_of(
					brush, brush + 3,
					std::bind(
						&Traits::SolidBrush::CreateResource,
						std::placeholders::_1,
						std::ref(cs)));
		}

		void DestroyResource() override
		{
			text.DestroyResource();
			std::for_each(brush, brush + 3, std::mem_fn(&Traits::SolidBrush::DestroyResource));
		}

		void Draw(const typename Traits::PaintStruct &ps) override
		{
			typename Traits::Rect(this->GetPosition(), this->GetSize()).Fill(ps, brush[state]);
			text.Draw(ps);
		}
	};

}
