#pragma once

namespace ChaoticLib{

	template <class Traits>
	class Button: public Traits::Object
	{
	public:
#ifndef DONT_USE_ENUM_CLASS
		enum class State: int{
			None,
			Hover,
			Push,
		};
#else
		struct State{
			static const int None = 0;
			static const int Hover = 1;
			static const int Push = 2;

			int state;

			State(int s): state(s)
			{
			}
			operator int()
			{
				return state;
			}
		};
#endif

	protected:
		State state;
		bool pushing;
		virtual void SetState(State s)
		{
			state = s;
		}

	public:
		Button(): state(State::None), pushing(false)
		{
		}

		virtual void OnPush() = 0;

		virtual void OnLeftPress(const typename Traits::Point &ap, typename Traits::Object::HitTestStruct &hts) override
		{
			hts.SetCursor(HitTestStruct::Cursor::Hand);
			pushing = true;
			SetState(State::Push);
		}

		virtual void OnMouseMove(const typename Traits::Point &ap, typename Traits::Object::HitTestStruct &hts) override
		{
			if(pushing){
				hts.SetCursor(HitTestStruct::Cursor::Hand);
				if(IsColliding(ap)){
					// 押されててカーソルが中にある
					SetState(State::Push);
				}else{
					// 押されてるけど外にある
					SetState(State::Hover);
				}
			}else{
				if(IsColliding(ap)){
					// 押してないけどカーソルが乗ってる=ホバーしてる
					SetState(State::Hover);
					hts.SetCursor(HitTestStruct::Cursor::Hand);
				}else
					SetState(State::None);
			}
		}

		virtual void OnLeftRelease(const typename Traits::Point &ap, typename Traits::Object::HitTestStruct &hts) override
		{
			pushing = false;
			if(IsColliding(ap)){
				OnPush();
				SetState(State::Hover);
			}else
				SetState(State::None);
		}
		virtual void OnMouseLeave(const typename Traits::Object::HitTestStruct &hts)
		{
			SetState(State::None);
		}
		virtual void OnLoseFocus(const typename Traits::Object::HitTestStruct &hts)
		{
			SetState(State::None);
		}
	};

}
