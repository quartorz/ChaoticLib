#pragma once

#include <Windows.h>
#include <WindowsX.h>

#include <functional>
#include <tuple>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <ctime>
#include <random>

namespace ChaoticLib{ namespace Win32{

	template <class Derived>
	class Resizable
	{
	public:
		bool WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT &lresult)
		{
			if(msg == WM_SIZE)
				static_cast<Derived*>(this)->OnSize(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			return true;
		}

		void Resize(int w, int h)
		{
			RECT rc;
			::GetWindowRect(hwnd, &rc);
			POINT pt = {rc.left, rc.top};
			::ScreenToClient(hparent, &pt);
			::MoveWindow(hwnd, pt.x, pt.y, w, h, TRUE);
		}
	};

	template <class Derived>
	class Movable
	{
	public:
		bool WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT &lresult)
		{
			if(msg == WM_MOVE)
				static_cast<Derived*>(this)->OnMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			return true;
		}

		void Move(int x, int y)
		{
			RECT rc;
			::GetWindowRect(hwnd, &rc);
			::MoveWindow(hwnd, x, y, rc.right - rc.left, rc.bottom - rc.top, TRUE);
		}
	};

	template <class Derived>
	class QuitOnClose
	{
	public:
		bool WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT &lresult)
		{
			if(msg == WM_CLOSE)
				::PostQuitMessage(0);
			return true;
		}
	};

	template <class Derived>
	class LeftButton
	{
	public:
		bool WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT &lresult)
		{
			switch(msg){
			case WM_LBUTTONDOWN:
				static_cast<Derived*>(this)->OnLeftPress(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				break;
			case WM_LBUTTONUP:
				static_cast<Derived*>(this)->OnLeftRelease(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				break;
			}

			return true;
		}
	};

	template <class Derived>
	class RightButton
	{
	public:
		bool WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT &lresult)
		{
			switch(msg){
			case WM_RBUTTONDOWN:
				static_cast<Derived*>(this)->OnRightPress(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				break;
			case WM_RBUTTONUP:
				static_cast<Derived*>(this)->OnRightRelease(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				break;
			}

			return true;
		}
	};

	template <class Derived>
	class MouseMove
	{
	public:
		bool WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT &lresult)
		{
			if(msg == WM_MOUSEMOVE)
				static_cast<Derived*>(this)->OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

			return true;
		}
	};

	template <class Derived>
	class Keyboard
	{
		using handler_type = std::function<void(unsigned keycode, bool is_push)>;
		using hash_type = std::mt19937_64::result_type;
		using tuple_type = std::tuple<hash_type, handler_type>;
		using container_type = std::vector<tuple_type>;
		using map_type = std::unordered_map<unsigned, container_type>;
		map_type map;

		std::mt19937_64 rand{std::time(nullptr)};

	public:
		bool WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT &lresult)
		{
			switch(msg){
			case WM_KEYDOWN:
				{
					auto it = map.find(static_cast<unsigned>(wParam));
					if(it != map.end()){
						auto &container = it->second;
						for(auto &tuple: container){
							std::get<1>(tuple)(wParam, true);
						}
					}
				}
				static_cast<Derived*>(this)->OnKeyDown(wParam);
				break;
			case WM_KEYUP:
				{
					auto it = map.find(static_cast<unsigned>(wParam));
					if(it != map.end()){
						auto &container = it->second;
						for(auto &tuple : container){
							std::get<1>(tuple)(wParam, false);
						}
					}
				}
				static_cast<Derived*>(this)->OnKeyUp(wParam);
				break;
			}

			return true;
		}

		template <class Type, class... Types>
		hash_type AddKeyboardHandler(handler_type handler, Type t, Types... characters)
		{
			hash_type hash = rand();
			auto tuple = tuple_type(hash, handler);

			AddHandler(tuple, t, characters...);

			return hash;
		}

		template <class... Types>
		void DeleteKeyboardHandler(hash_type hash, Types... characters)
		{
			DeleteHandler(hash, characters...);
		}

	private:
		void Add(tuple_type &tuple, wchar_t c)
		{
			map[c].push_back(tuple);
		}

		void AddHandler(tuple_type &)
		{
		}

		template <class... Others>
		void AddHandler(tuple_type &tuple, wchar_t c, Others... others)
		{
			Add(tuple, c);
			AddHandler(tuple, others...);
		}

		template <class... Others>
		void AddHandler(tuple_type &tuple, std::tuple<wchar_t, wchar_t> &range, Others... others)
		{
			wchar_t start = std::get<0>(range);
			wchar_t end = std::get<1>(range);
			if(start <= end){
				do{
					Add(tuple, start);
				} while(++start <= end);
			} else{
				do{
					Add(tuple, start);
				} while(--start >= end);
			}
			AddHandler(tuple, others...);
		}

		void Delete(hash_type hash, wchar_t c)
		{
			auto it = map.find(c);
			if(it == map.end())
				return;

			auto &container = it->second;
			auto iter = std::find_if(container.begin(), container.end(), [&hash](container_type::value_type &v){return std::get<0>(v) == hash; });
			if(iter != container.end())
				container.erase(iter);
		}

		void DeleteHandler(hash_type)
		{
		}

		template <class... Others>
		void DeleteHandler(hash_type hash, wchar_t c, Others... others)
		{
			Delete(hash, c);
			DeleteHandler(hash, others...);
		}

		template <class... Others>
		void DeleteHandler(hash_type hash, std::tuple<wchar_t, wchar_t> &range, Others... others)
		{
			wchar_t start = std::get<0>(range);
			wchar_t end = std::get<1>(range);
			if(start <= end){
				do{
					Delete(hash, start);
				} while(++start <= end);
			} else{
				do{
					Delete(hash, start);
				} while(--start >= end);
			}
			DeleteHandler(hash, others...);
		}
	};

	template <class Derived>
	class Timer
	{
		using handler_type = std::function<void(unsigned id)>;
		using hash_type = std::mt19937_64::result_type;
		using tuple_type = std::tuple<hash_type, handler_type>;
		using id_type = unsigned;
		using container_type = std::vector<tuple_type>;
		using map_type = std::unordered_map<id_type, container_type>;
		map_type map;

		std::mt19937_64 rand{std::time(nullptr)};

	public:
		bool WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT &lresult)
		{
			if(msg == WM_TIMER){
				auto it = map.find(wParam);
				if(it != map.end()){
					auto &container = it->second;
					for(auto &tuple: container){
						std::get<1>(tuple)(wParam);
					}
				}
				static_cast<Derived*>(this)->OnTimer(wParam);
			}

			return true;
		}

		bool SetTimer(unsigned id, UINT time)
		{
			return ::SetTimer(static_cast<Derived*>(this)->GetHwnd(), id, time, NULL) != 0;
		}

		void KillTimer(unsigned id)
		{
			::KillTimer(static_cast<Derived*>(this)->GetHwnd(), id);
		}

		template <class Integer, class... Integers>
		hash_type AddTimerHandler(handler_type handler, Integer id, Integers... ids)
		{
			hash_type hash = rand();
			AddHandler(tuple_type(hash, handler), id, ids...);

			return hash;
		}

		template <class... Integers>
		void DeleteTimerHandler(hash_type hash, Integers... ids)
		{
			DeleteHandler(hash, ids...);
		}

	private:
		void AddHandler(tuple_type &)
		{
		}

		template <class... Integers>
		void AddHandler(tuple_type &tuple, unsigned id, Integers... ids)
		{
			map[id].push_back(tuple);
			AddHandler(tuple, ids...);
		}

		void DeleteHandler(hash_type)
		{
		}

		template <class... Integers>
		void DeleteHandler(hash_type hash, unsigned id, Integers... ids)
		{
			auto it = map.find(id);
			if(it == map.end())
				return;

			auto &container = it->second;
			auto iter = std::find_if(container.begin(), container.end(), [&hash](container_type::value_type &v){return std::get<0>(v) == hash;});
			if(iter != container.end())
				container.erase(iter);
		}
	};

} }
