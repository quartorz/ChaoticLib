#pragma once

namespace ChaoticLib{

	template <class Window, class Traits>
	class Scene: public Traits::UserDefinedObject<Scene<Window, Traits>, Window>::type
	{
		typedef typename Traits::UserDefinedObject<Scene<Window, Traits>, Window>::type super;

	public:
		Scene(Window *w): super(w)
		{
		}
		virtual void Show()
		{
		}
		virtual void Hide()
		{
		}
		virtual void OnKeyDown(unsigned)
		{
		}
		virtual void OnKeyUp(unsigned)
		{
		}
	};

}
