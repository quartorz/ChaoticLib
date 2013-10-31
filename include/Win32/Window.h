#pragma once

#include <Windows.h>
#include <WindowsX.h>

#include <boost\preprocessor\facilities\intercept.hpp>
#include <boost\preprocessor\enum_params.hpp>
#include <boost\preprocessor\repeat_from_to.hpp>
#include <boost\preprocessor\comparison\not_equal.hpp>
#include <boost\preprocessor\repetition\enum_binary_params.hpp>
#include <boost\preprocessor\arithmetic\dec.hpp>
#include <boost\preprocessor\arithmetic\sub.hpp>

#include <tuple>

#include "..\Nil.h"

namespace ChaoticLib{ namespace Win32{

#define PROC_MAX 50

#define DECLARE(n) \
	{ \
		static LRESULT CALLBACK WindowProc_SetData(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) \
		{ \
			auto w = reinterpret_cast<Derived*>(::GetWindowLongPtrW(hwnd, 0)); \
			if(msg == WM_NCCREATE){ \
				auto lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam); \
				::SetWindowLongPtrW(hwnd, 0, reinterpret_cast<LONG_PTR>(lpcs->lpCreateParams)); \
				::SetWindowLongPtrW(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WindowProc_Static)); \
				w = reinterpret_cast<Derived*>(lpcs->lpCreateParams); \
				w->hwnd = hwnd; \
				w->hparent = lpcs->hwndParent; \
			} \
			if(w == nullptr) \
				return ::DefWindowProcW(hwnd, msg, wParam, lParam); \
			return w->WindowProc(hwnd, msg, wParam, lParam); \
		} \
		 \
		static LRESULT CALLBACK WindowProc_Static(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) \
		{ \
			auto w = reinterpret_cast<Derived*>(::GetWindowLongPtrW(hwnd, 0)); \
			if(w == nullptr) \
				return ::DefWindowProcW(hwnd, msg, wParam, lParam); \
			return w->WindowProc(hwnd, msg, wParam, lParam); \
		} \
		 \
	protected: \
		HWND hwnd, hparent; \
		 \
	public: \
		Window(): hwnd(NULL), hparent(hwnd) \
		{ \
		} \
		 \
		static bool Register(const wchar_t *classname) \
		{ \
			WNDCLASSEXW wcex; \
			wcex.cbSize			= sizeof(WNDCLASSEX); \
			wcex.style			= CS_HREDRAW | CS_VREDRAW; \
			wcex.lpfnWndProc	= WindowProc_SetData; \
			wcex.cbClsExtra		= 0; \
			wcex.cbWndExtra		= sizeof(void*); \
			wcex.hInstance		= ::GetModuleHandleW(NULL); \
			wcex.hIcon			= nullptr; \
			wcex.hCursor		= LoadCursor(nullptr, IDC_ARROW); \
			wcex.hbrBackground	= nullptr; \
			wcex.lpszMenuName	= nullptr; \
			wcex.lpszClassName	= classname; \
			wcex.hIconSm		= nullptr; \
			 \
			return ::RegisterClassExW(&wcex) != 0; \
		} \
		 \
		static bool Register(void) \
		{ \
			return Register(Derived::classname); \
		} \
		 \
		HWND GetHwnd() const \
		{ \
			return hwnd; \
		} \
		 \
		void Show(void) \
		{ \
			::ShowWindow(hwnd, SW_SHOW); \
		} \
		 \
		void Hide(void) \
		{ \
			::ShowWindow(hwnd, SW_HIDE); \
		} \
		 \
		std::tuple<int, int> GetSize() \
		{ \
			RECT rc; \
			::GetClientRect(hwnd, &rc); \
			return std::tuple<int, int>(rc.right, rc.bottom);\
		} \
		 \
		std::tuple<int, int> GetPosition() \
		{ \
			RECT rc; \
			::GetClientRect(hwnd, &rc); \
			return std::tuple<int, int>(rc.left, rc.top); \
		} \
		 \
		LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) \
		{ \
			LRESULT lresult = 0l; \
			BOOST_PP_REPEAT(n, CALL_PROC,) \
			 \
			return ::DefWindowProcW(hwnd, msg, wParam, lParam); \
		} \
	};

#define CALL_PROC(z, n, _) \
	if(!static_cast<Proc ## n*>(this)->WindowProc(hwnd, msg, wParam, lParam, lresult)) \
		return lresult;

	template <class Derived, BOOST_PP_ENUM_BINARY_PARAMS(PROC_MAX, class Proc, = ChaoticLib::Nil BOOST_PP_INTERCEPT)>
	class Window
		DECLARE(0)

#define WINDOW(z, n, d) \
	template <class Derived, BOOST_PP_ENUM_PARAMS(n, class Proc)> \
	class Window<Derived, BOOST_PP_ENUM_PARAMS(n, Proc) BOOST_PP_COMMA_IF(BOOST_PP_NOT_EQUAL(n, PROC_MAX)) BOOST_PP_ENUM_PARAMS(BOOST_PP_SUB(PROC_MAX, n), ChaoticLib::Nil BOOST_PP_INTERCEPT)>: \
		BOOST_PP_ENUM_PARAMS(n, public Proc) \
		DECLARE(n)

	WINDOW(_, 1, _)
	WINDOW(_, 2, _)
	WINDOW(_, 3, _)
	WINDOW(_, 4, _)
	WINDOW(_, 5, _)
	WINDOW(_, 6, _)
	WINDOW(_, 7, _)
	WINDOW(_, 8, _)
	WINDOW(_, 9, _)
	WINDOW(_, 10, _)
	WINDOW(_, 11, _)
	WINDOW(_, 12, _)
	WINDOW(_, 13, _)
	WINDOW(_, 14, _)
	WINDOW(_, 15, _)
	WINDOW(_, 16, _)
	WINDOW(_, 17, _)
	WINDOW(_, 18, _)
	WINDOW(_, 19, _)
	WINDOW(_, 20, _)
	WINDOW(_, 21, _)
	WINDOW(_, 22, _)
	WINDOW(_, 23, _)
	WINDOW(_, 24, _)
	WINDOW(_, 25, _)
	WINDOW(_, 26, _)
	WINDOW(_, 27, _)
	WINDOW(_, 28, _)
	WINDOW(_, 29, _)
	WINDOW(_, 30, _)
	WINDOW(_, 31, _)
	WINDOW(_, 32, _)
	WINDOW(_, 33, _)
	WINDOW(_, 34, _)
	WINDOW(_, 35, _)
	WINDOW(_, 36, _)
	WINDOW(_, 37, _)
	WINDOW(_, 38, _)
	WINDOW(_, 39, _)
	WINDOW(_, 40, _)
	WINDOW(_, 41, _)
	WINDOW(_, 42, _)
	WINDOW(_, 43, _)
	WINDOW(_, 44, _)
	WINDOW(_, 45, _)
	WINDOW(_, 46, _)
	WINDOW(_, 47, _)
	WINDOW(_, 48, _)
	WINDOW(_, 49, _)

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

} }
