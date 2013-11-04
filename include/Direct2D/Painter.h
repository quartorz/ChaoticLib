#pragma once

#include <d2d1.h>
#include <cstring>

#include "Factory.h"
#include "BaseTypes.h"
#include "Brush.h"
#include "Font.h"

#undef DrawText

namespace ChaoticLib{ namespace Direct2D{

	template <class Derived>
	class Painter{
		Factory factory;
		ID2D1HwndRenderTarget *target;
		CRITICAL_SECTION cs;
		inline HWND Hwnd()
		{
			return static_cast<Derived*>(this)->GetHwnd();
		}
		bool CreateResource_();
		void DestroyResource_();

	public:
		Resource::CreateStruct CreateStruct()
		{
			Resource::CreateStruct cd = {
				target
			};
			return cd;
		}
		void DrawLine(const PaintStruct &ps, const Brush &brush, const Point &start, const Point &end);
		void DrawLine(const PaintStruct &ps, const Brush *brush, const Point &start, const Point &end)
		{
			DrawLine(ps, *brush, start, end);
		}
		void DrawRect(const PaintStruct &ps, const Brush &brush, const Rect &rect);
		void DrawRect(const PaintStruct &ps, const Brush *brush, const Rect &rect)
		{
			DrawRect(ps, *brush, rect);
		}
		void DrawText(const PaintStruct &ps, const Font &font, const Brush &brush, const wchar_t *text, const Rect &rect, bool clip=false);
		void DrawText(const PaintStruct &ps, const Font *font, const Brush &brush, const wchar_t *text, const Rect &rect, bool clip=false)
		{
			DrawText(ps, *font, brush, text, rect, clip);
		}
		void DrawText(const PaintStruct &ps, const Font &font, const Brush *brush, const wchar_t *text, const Rect &rect, bool clip=false)
		{
			DrawText(ps, font, *brush, text, rect, clip);
		}
		void DrawText(const PaintStruct &ps, const Font *font, const Brush *brush, const wchar_t *text, const Rect &rect, bool clip=false)
		{
			DrawText(ps, *font, *brush, text, rect, clip);
		}
		void Clear(const PaintStruct &ps, const Color &c);

		typedef PaintStruct PaintStruct;

		Painter(): target(nullptr)
		{
			::InitializeCriticalSection(&cs);
		}
		~Painter()
		{
			SafeRelease(target);
			::DeleteCriticalSection(&cs);
		}

		void Repaint();

		void Lock();
		void Unlock();

		bool WindowProc(HWND, UINT, WPARAM, LPARAM, LRESULT &);
		void OnPaint();
	};

	template <class Derived>
	inline bool Painter<Derived>::CreateResource_()
	{
		if(target != nullptr)
			return true;

		RECT rc;
		::GetClientRect(Hwnd(), &rc);
		HRESULT hr = factory[D2D]->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(
				Hwnd(),
				D2D1::SizeU(rc.right, rc.bottom)),
			&target);
		if(FAILED(hr))
			return false;

		return static_cast<Derived*>(this)->CreateResource(CreateStruct());
	}

	template <class Derived>
	inline void Painter<Derived>::DestroyResource_()
	{
		static_cast<Derived*>(this)->DestroyResource();
		SafeRelease(target);
	}

	template <class Derived>
	inline void Painter<Derived>::DrawLine(const PaintStruct &ps, const Brush &brush, const Point &start, const Point &end)
	{
		ps.target->DrawLine(start, end, brush.Get());
	}

	template <class Derived>
	inline void Painter<Derived>::DrawRect(const PaintStruct &ps, const Brush &brush, const Rect &rect)
	{
		ps.target->FillRectangle(
			rect,
			brush.Get());
	}

	template <class Derived>
	inline void Painter<Derived>::DrawText(const PaintStruct &ps, const Font &font, const Brush &brush, const wchar_t *text, const Rect &rect, bool clip)
	{
		ps.target->DrawTextW(
			text,
			std::wcslen(text),
			font.Get(),
			rect,
			brush.Get(),
			(clip ? D2D1_DRAW_TEXT_OPTIONS_CLIP: D2D1_DRAW_TEXT_OPTIONS_NONE));
	}

	template <class Derived>
	inline void Painter<Derived>::Clear(const PaintStruct &ps, const Color &c)
	{
		ps.target->Clear(c);
	}

	template <class Derived>
	inline void Painter<Derived>::Repaint()
	{
		::InvalidateRect(Hwnd(), nullptr, FALSE);
	}

	template <class Derived>
	inline void Painter<Derived>::Lock()
	{
		::EnterCriticalSection(&cs);
	}

	template <class Derived>
	inline void Painter<Derived>::Unlock()
	{
		::LeaveCriticalSection(&cs);
	}

	template <class Derived>
	inline bool Painter<Derived>::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT &r)
	{
		switch(msg){
		case WM_PAINT:
			OnPaint();
			break;
		case WM_ERASEBKGND:
			break;
		case WM_SIZE:
			if(target != nullptr)
				target->Resize(D2D1::SizeU(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
			break;
		}
		return true;
	}

	template <class Derived>
	inline void Painter<Derived>::OnPaint()
	{
		Lock();

		if(CreateResource_()){
			PAINTSTRUCT ps;

			::BeginPaint(Hwnd(), &ps);
			::EndPaint(Hwnd(), &ps);

			PaintStruct p = {
				target, false
			};

			target->BeginDraw();
			static_cast<Derived*>(this)->Draw(p);
			HRESULT hr = target->EndDraw();
			if(hr == D2DERR_RECREATE_TARGET){
				DestroyResource_();
				::InvalidateRect(Hwnd(), nullptr, FALSE);
			}
		}else{
			throw std::runtime_error("an error occurred when creating Direct2D resources.");
		}

		Unlock();
	}

} }
