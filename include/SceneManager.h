#pragma once

#include "Scene.h"

#include <deque>

namespace ChaoticLib{

	template <class Derived, class Traits>
	class SceneManager{
		std::deque<Scene<Derived, Traits>*> scenes;
		int selected;
		bool created;

	public:
		SceneManager(): selected(0), created(false)
		{
		}

		void AddScene(Scene<Derived, Traits> *scene)
		{
			if(created)
				scene->CreateResource(static_cast<Derived*>(this)->CreateStruct());
			scenes.push_back(scene);
		}

		void SelectScene(int s)
		{
			scenes[selected]->Hide();
			scenes[s]->Show();
			selected = s;
		}

		int GetSceneCount() const
		{
			scenes.size();
		}

		void OnSize(int w, int h)
		{
			typedef typename Traits::Size Size;
			std::for_each(
				scenes.begin(), scenes.end(),
				[&w, &h, this](Scene<Derived, Traits> *scene){
					scene->SetSize(Size(w, h));
			});
		}

		void OnKeyDown(unsigned keycode)
		{
			scenes[selected]->OnKeyDown(keycode);
		}

		void OnKeyUp(unsigned keycode)
		{
			scenes[selected]->OnKeyUp(keycode);
		}

		bool CreateResource(const typename Traits::CreateStruct &cs)
		{
			created = true;
			return std::all_of(
				scenes.begin(), scenes.end(),
				std::bind(
					&Scene<Derived, Traits>::CreateResource,
					std::placeholders::_1,
					std::ref(cs)));
		}

		void DestroyResource()
		{
			created = false;
			std::for_each(scenes.begin(), scenes.end(), std::mem_fn(&Scene<Derived, Traits>::DestroyResource));
		}

		void Draw(const typename Traits::PaintStruct &ps)
		{
			static_cast<Derived*>(this)->Clear(ps, typename Traits::Color(255, 255, 255));
			scenes[selected]->Draw(ps);
		}

		template <class Func>
		void IterateObjects(Func func){
			func(*scenes[selected]);
		}
	};

}
