#pragma once

#include <d2d1.h>
#include <dwmapi.h>
#include <WindowsX.h>
#include <vsstyle.h>
#include "BaseTypes.h"
#include "Factory.h"
#include "Object.h"
#include "Brush.h"
#include "..\Macro.h"

namespace ChaoticLib{ namespace Direct2D{

	template <class Derived>
	class Painter{
		inline HWND Hwnd()
		{
			return static_cast<Derived*>(this)->GetHwnd();
		}

		Factory factory;
		HTHEME htheme;
		ID2D1DCRenderTarget *target;
		CRITICAL_SECTION cs;

		bool CreateResource_();
		void DestroyResource_();

	public:
		typedef PaintStruct PaintStruct;

		ChaoticLib::Direct2D::CreateStruct CreateStruct()
		{
			ChaoticLib::Direct2D::CreateStruct cs ={
				target
			};

			return cs;
		}
		void DrawLine(const PaintStruct &ps, const Brush &brush, Line line);
		void DrawLine(const PaintStruct &ps, const Brush *brush, const Line &line)
		{
			DrawLine(ps, *brush, line);
		}
		void DrawRect(const PaintStruct &ps, const Brush &brush, const Rect &rect);
		void DrawRect(const PaintStruct &ps, const Brush *brush, const Rect &rect)
		{
			DrawRect(ps, *brush, rect);
		}
		void DrawCircle(const PaintStruct &ps, const Brush &brush, Circle circle);
		void DrawCircle(const PaintStruct &ps, const Brush *brush, const Circle &circle)
		{
			DrawCircle(ps, brush, circle);
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

		Painter():
			htheme(nullptr),
			target(nullptr)
		{
			::InitializeCriticalSection(&cs);
		}
		~Painter()
		{
			SafeRelease(target);
			::DeleteCriticalSection(&cs);
		}

		void Repaint();

		void Lock()
		{
			::EnterCriticalSection(&cs);
		}
		void Unlock()
		{
			::LeaveCriticalSection(&cs);
		}

		bool WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT &r);

		void OnPaint();
	};


	template <class Derived>
	inline bool Painter<Derived>::CreateResource_()
	{
		if(target != nullptr)
			return true;

		auto props = D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_DEFAULT,
			D2D1::PixelFormat(
			DXGI_FORMAT_B8G8R8A8_UNORM,
			D2D1_ALPHA_MODE_PREMULTIPLIED),
			0,
			0,
			D2D1_RENDER_TARGET_USAGE_NONE,
			D2D1_FEATURE_LEVEL_DEFAULT);
		if(FAILED(factory[D2D]->CreateDCRenderTarget(
			&props,
			&target)))
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
	inline void Painter<Derived>::DrawLine(const PaintStruct &ps, const Brush &brush, Line line)
	{
		ps.target->DrawLine(line.GetStartPoint(), line.GetEndPoint(), brush.Get());
	}

	template <class Derived>
	inline void Painter<Derived>::DrawRect(const PaintStruct &ps, const Brush &brush, const Rect &rect)
	{
		ps.target->FillRectangle(
			rect,
			brush.Get());
	}

	template <class Derived>
	inline void Painter<Derived>::DrawCircle(const PaintStruct &ps, const Brush &brush, Circle circle)
	{
		ps.target->DrawEllipse(
			D2D1::Ellipse(circle.GetCenter(), circle.GetRadius(), circle.GetRadius()),
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
			(clip ? D2D1_DRAW_TEXT_OPTIONS_CLIP : D2D1_DRAW_TEXT_OPTIONS_NONE));
	}

	template <class Derived>
	inline void Painter<Derived>::Clear(const PaintStruct &ps, const Color &c)
	{
		ps.target->Clear(c);
	}

	template <class Derived>
	inline void Painter<Derived>::Repaint()
	{
		::InvalidateRect(Hwnd(), NULL, FALSE);
	}

	template <class Derived>
	inline bool Painter<Derived>::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT &r)
	{
		switch(msg){
		case WM_CREATE:
			{
				DWM_BLURBEHIND bb ={};
				bb.dwFlags = DWM_BB_ENABLE;
				bb.fEnable = TRUE;
				::DwmEnableBlurBehindWindow(
					Hwnd(),
					&bb);
				htheme = ::OpenThemeData(Hwnd(), VSCLASS_WINDOW);
			}
			break;
		case WM_DESTROY:
			::CloseThemeData(htheme);
			this->DestroyResource_();
			break;
		case WM_PAINT:
			this->OnPaint();
			break;
		case WM_SIZE:
			::InvalidateRect(Hwnd(), NULL, FALSE);
			break;
		}
		return true;
	}

	template <class Derived>
	inline void Painter<Derived>::OnPaint()
	{
		if(!this->CreateResource_())
			throw std::runtime_error("an error occurred when creating Direct2D resources.");

		this->Lock();

		PaintStruct d = {};

		d.target = target;

		HWND hwnd = Hwnd();
		RECT rc;
		::GetClientRect(hwnd, &rc);

		PAINTSTRUCT ps;
		::BeginPaint(hwnd, &ps);

		target->BindDC(ps.hdc, &rc);
		target->BeginDraw();

		static_cast<Derived*>(this)->Draw(d);

		HRESULT hr = target->EndDraw();
		if(hr == D2DERR_RECREATE_TARGET){
			this->DestroyResource_();
			::InvalidateRect(Hwnd(), nullptr, FALSE);
		}

		::EndPaint(hwnd, &ps);

		this->Unlock();
	}

} }
