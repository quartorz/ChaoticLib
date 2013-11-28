#pragma once

#include <functional>
#include <tuple>
#include <vector>
#include <array>
#include <unordered_map>
#include <algorithm>
#include <ctime>
#include <random>
#include <string>
#include <cassert>

#if !defined DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif

#include <dinput.h>

namespace ChaoticLib{

	template <class Window, class Traits>
	class Scene: public Traits::template UserDefinedObject<Scene<Window, Traits>, Window>
	{
		typedef typename Traits::template UserDefinedObject<Scene<Window, Traits>, Window> Base;

		using hash_type = std::mt19937_64::result_type;

		using kb_handler_type = std::function<void(unsigned keycode, bool is_push)>;
		using kb_tuple_type = std::tuple<hash_type, kb_handler_type>;
		using kb_container_type = std::vector<kb_tuple_type>;
		using kb_map_type = std::array<kb_container_type, 256>;
		kb_map_type kb_map;

		using timer_handler_type = std::function<void(unsigned id)>;
		using timer_tuple_type = std::tuple<hash_type, timer_handler_type>;
		using timer_id_type = unsigned;
		using timer_container_type = std::vector<timer_tuple_type>;
		using timer_map_type = std::unordered_map<timer_id_type, timer_container_type>;
		timer_map_type timer_map;

		std::mt19937_64 rand{std::time(nullptr)};

	protected:
		using joystick_id = GUID;
		using keycode_range = std::tuple<wchar_t, wchar_t>;

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
		virtual void OnKeyDown(unsigned keycode)
		{
			assert(0 <= keycode && keycode <= 255);

			for(auto &tuple : kb_map[keycode]){
				std::get<1>(tuple)(keycode, true);
			}
		}
		virtual void OnKeyUp(unsigned keycode)
		{
			assert(0 <= keycode && keycode <= 255);

			for(auto &tuple: kb_map[keycode]){
				std::get<1>(tuple)(keycode, false);
			}
		}
		virtual void OnTimer(unsigned id)
		{
			auto it = timer_map.find(id);
			if(it != timer_map.end()){
				auto &container = it->second;
				for(auto &tuple: container){
					std::get<1>(tuple)(id);
				}
			}
		}
		virtual void OnGetJoystickState(const joystick_id &id, const DIJOYSTATE2 &js)
		{
		}
		virtual void OnReloadJoysticks(const std::vector<std::tuple<joystick_id, std::wstring>> &joysticks)
		{
		}

		template <class Type, class... Types>
		hash_type AddKeyboardHandler(kb_handler_type handler, Type t, Types... characters)
		{
			hash_type hash = rand();
			auto tuple = kb_tuple_type(hash, handler);

			AddKeyboardHandlerHelper(tuple, t, characters...);

			return hash;
		}

		template <class... Types>
		void DeleteKeyboardHandler(hash_type hash, Types... characters)
		{
			DeleteKeyboardHandlerHelper(hash, characters...);
		}

		template <class Integer, class... Integers>
		hash_type AddTimerHandler(timer_handler_type handler, Integer id, Integers... ids)
		{
			hash_type hash = rand();
			AddTimerHandlerHelper(timer_tuple_type(hash, handler), id, ids...);

			return hash;
		}

		template <class... Integers>
		void DeleteTimerHandler(hash_type hash, Integers... ids)
		{
			DeleteTimerHandlerHelper(hash, ids...);
		}

	private:
		void AddKeyboardHandlerHelper2(kb_tuple_type &tuple, wchar_t c)
		{
			assert(0 <= c && c <= 255);
			kb_map[c].push_back(tuple);
		}

		void AddKeyboardHandlerHelper(kb_tuple_type &)
		{
		}

		template <class... Others>
		void AddKeyboardHandlerHelper(kb_tuple_type &tuple, wchar_t c, Others... others)
		{
			AddKeyboardHandlerHelper2(tuple, c);
			AddKeyboardHandlerHelper(tuple, others...);
		}

		template <class... Others>
		void AddKeyboardHandlerHelper(kb_tuple_type &tuple, std::tuple<wchar_t, wchar_t> &range, Others... others)
		{
			wchar_t start = std::get<0>(range);
			wchar_t end = std::get<1>(range);
			if(start <= end){
				do{
					AddKeyboardHandlerHelper2(tuple, start);
				} while(++start <= end);
			} else{
				do{
					AddKeyboardHandlerHelper2(tuple, start);
				} while(--start >= end);
			}
			AddKeyboardHandlerHelper(tuple, others...);
		}

		void DeleteKeyboardHandlerHelper2(hash_type hash, wchar_t c)
		{
			auto it = kb_map.find(c);
			if(it == kb_map.end())
				return;

			auto &container = it->second;
			auto iter = std::find_if(container.begin(), container.end(), [&hash](kb_container_type::value_type &v){return std::get<0>(v) == hash; });
			if(iter != container.end())
				container.erase(iter);
		}

		void DeleteKeyboardHandlerHelper(hash_type)
		{
		}

		template <class... Others>
		void DeleteKeyboardHandlerHelper(hash_type hash, wchar_t c, Others... others)
		{
			DeleteKeyboardHandlerHelper2(hash, c);
			DeleteKeyboardHandlerHelper(hash, others...);
		}

		template <class... Others>
		void DeleteKeyboardHandlerHelper(hash_type hash, std::tuple<wchar_t, wchar_t> &range, Others... others)
		{
			wchar_t start = std::get<0>(range);
			wchar_t end = std::get<1>(range);
			if(start <= end){
				do{
					DeleteKeyboardHandlerHelper2(hash, start);
				} while(++start <= end);
			} else{
				do{
					DeleteKeyboardHandlerHelper2(hash, start);
				} while(--start >= end);
			}
			DeleteKeyboardHandlerHelper(hash, others...);
		}

		void AddTimerHandlerHelper(timer_tuple_type &)
		{
		}

		template <class... Integers>
		void AddTimerHandlerHelper(timer_tuple_type &tuple, unsigned id, Integers... ids)
		{
			timer_map[id].push_back(tuple);
			AddTimerHandlerHelper(tuple, ids...);
		}

		void DeleteTimerHandlerHandler(hash_type)
		{
		}

		template <class... Integers>
		void DeleteTimerHandlerHandler(hash_type hash, unsigned id, Integers... ids)
		{
			auto it = timer_map.find(id);
			if(it == timer_map.end())
				return;

			auto &container = it->second;
			auto iter = std::find_if(container.begin(), container.end(), [&hash](timer_container_type::value_type &v){return std::get<0>(v) == hash; });
			if(iter != container.end())
				container.erase(iter);
		}
	};

}
