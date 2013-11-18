#pragma once

#include "Scene.h"

#include <unordered_map>
#include <vector>
#include <type_traits>
#include <cstdint>

#if !defined DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif

#include <dinput.h>

namespace ChaoticLib{

	template <class Derived, class Traits>
	class SceneManager{
		std::unordered_map<int, Scene<Derived, Traits>*> scenes;
		Scene<Derived, Traits> *scene;
		int selected;
		bool created;

		std::int64_t hash = 0;

	public:
		SceneManager(): scene(nullptr), selected(0), created(false)
		{
		}

		void AddScene(int key, Scene<Derived, Traits> *scene)
		{
			if(created)
				scene->CreateResource(static_cast<Derived*>(this)->CreateStruct());
			scenes.insert({key, scene});
			if(this->scene == nullptr)
				this->scene = scene;
		}

		void SelectScene(int key)
		{
			scenes[selected]->Hide();
			scene = scenes[key];
			scene->Show();
			selected = key;
			static_cast<Derived*>(this)->Repaint();
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
				[&w, &h, this](const std::pair<int, Scene<Derived, Traits>*> &p){
					p.second->SetSize(Size(w, h));
			});
		}

		void OnKeyDown(unsigned keycode)
		{
			scene->OnKeyDown(keycode);
		}

		void OnKeyUp(unsigned keycode)
		{
			scene->OnKeyUp(keycode);
		}

		void OnTimer(unsigned id)
		{
			scene->OnTimer(id);
		}

		void OnGetJoystickState(GUID &guid, DIJOYSTATE2 &js)
		{
			scene->OnGetJoystickState(guid, js);
		}

		void OnReloadJoystick(const std::vector<GUID> &guids)
		{
			for(auto &scene: scenes){
				std::get<1>(scene)->OnReloadJoystick(guids);
			}
		}

		bool CreateResource(const typename Traits::CreateStruct &cs)
		{
			created = true;
			return std::all_of(
				scenes.begin(), scenes.end(),
				[&cs](const std::pair<int, Scene<Derived, Traits>*> &p){
					return p.second->CreateResource(cs);
			});
		}

		void DestroyResource()
		{
			created = false;
			std::for_each(scenes.begin(), scenes.end(), [](const std::pair<int, Scene<Derived, Traits>*> &p){p.second->DestroyResource();});
		}

		void Draw(const typename Traits::PaintStruct &ps)
		{
			static_cast<Derived*>(this)->Clear(ps, typename Traits::Color(255, 255, 255));
			scene->Draw(ps);
		}

		template <class Func>
		void IterateObjects(Func func){
			func(*scene);
		}
	};

}
