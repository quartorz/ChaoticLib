#pragma once

#include <initializer_list>
#include <functional>
#include <tuple>
#include <vector>
#include <unordered_map>
#include <ctime>
#include <algorithm>

namespace ChaoticLib{

	template <class Window, class Traits>
	class Scene: public Traits::template UserDefinedObject<Scene<Window, Traits>, Window>
	{
		typedef typename Traits::template UserDefinedObject<Scene<Window, Traits>, Window> Base;



	public:
		Scene(Window *w): Base(w)
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
		virtual void OnTimer(unsigned)
		{
		}
	};

}
