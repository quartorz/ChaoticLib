#pragma once

#include <Windows.h>
#include <WindowsX.h>

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
	public:
		bool WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT &lresult)
		{
			switch(msg){
			case WM_KEYDOWN:
				static_cast<Derived*>(this)->OnKeyDown(wParam);
				break;
			case WM_KEYUP:
				static_cast<Derived*>(this)->OnKeyUp(wParam);
				break;
			}

			return true;
		}
	};

	template <class Derived, UINT_PTR ID>
	class Timer
	{
	public:
		bool WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT &lresult)
		{
			if(msg == WM_TIMER && wParam == ID)
				static_cast<Derived*>(this)->OnTimer(ID);

			return true;
		}

		bool SetTimer(UINT time)
		{
			return ::SetTimer(static_cast<Derived*>(this)->GetHwnd(), ID, time, NULL) != 0;
		}

		void KillTimer()
		{
			::KillTimer(static_cast<Derived*>(this)->GetHwnd(), ID);
		}
	};

} }
