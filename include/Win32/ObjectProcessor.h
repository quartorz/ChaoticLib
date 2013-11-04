#pragma once

#include <Windows.h>
#include <WindowsX.h>

namespace ChaoticLib{ namespace Win32{

	template <class Derived, class Traits>
	class ObjectProcessor{
		HCURSOR hCursor[3];
		typename Traits::Object *focus, *hover;
		bool inside, pushing;

	protected:
		enum CursorType{
			Arrow,
			Hand,
			Ibeam
		}cursor_type;

		void SetCursorType(CursorType);

	public:
		ObjectProcessor(): cursor_type(Arrow), focus(nullptr), hover(nullptr), inside(false), pushing(false)
		{
			hCursor[0] = static_cast<HCURSOR>(::LoadImageW(
				nullptr,
				IDC_ARROW,
				IMAGE_CURSOR,
				0,
				0,
				LR_DEFAULTSIZE | LR_DEFAULTCOLOR | LR_SHARED));
			hCursor[1] = static_cast<HCURSOR>(::LoadImageW(
				nullptr,
				IDC_HAND,
				IMAGE_CURSOR,
				0,
				0,
				LR_DEFAULTSIZE | LR_DEFAULTCOLOR | LR_SHARED));
			hCursor[2] = static_cast<HCURSOR>(::LoadImageW(
				nullptr,
				IDC_IBEAM,
				IMAGE_CURSOR,
				0,
				0,
				LR_DEFAULTSIZE | LR_DEFAULTCOLOR | LR_SHARED));
		}

		~ObjectProcessor()
		{
			::DeleteObject(hCursor[0]);
			::DeleteObject(hCursor[1]);
			::DeleteObject(hCursor[2]);
		}

		bool WindowProc(HWND, UINT, WPARAM, LPARAM, LRESULT &);
	};

	template <class Derived, class Traits>
	void ObjectProcessor<Derived, Traits>::SetCursorType(CursorType t)
	{
		cursor_type = t;
		::SetCursor(hCursor[t]);
	}

	template <class Derived, class Traits>
	bool ObjectProcessor<Derived, Traits>::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT &l)
	{
		typedef typename Traits::Object Object;
		typedef typename Traits::Point Point;
		typedef typename Traits::CreateStruct CreateStruct;

		switch(msg){
		case WM_MOUSEMOVE:
			{
				inside = true;

				auto hts = Object::CreateHitTestStruct(static_cast<Derived*>(this));
				int x = GET_X_LPARAM(lParam);
				int y = GET_Y_LPARAM(lParam);
				
				if(pushing && focus != nullptr){
					focus->OnMouseMove(Point(x, y), hts);
				}else{
					if(hover != nullptr && !hover->IsColliding(Point(x, y))){
						hover->OnMouseLeave(hts);
						hover = nullptr;
					}
					static_cast<Derived*>(this)->IterateObjects([&](typename Object &o)-> bool{
						if(o.IsColliding(Point(x, y))){
							o.OnMouseMove(Point(x, y), hts);
							if(hover != nullptr && &o != hover){
								hover->OnMouseLeave(hts);
							}
							hover = &o;
							return true;
						}
						return false;
					});
				}

				SetCursorType(static_cast<CursorType>(static_cast<int>(hts.cursor)));

				TRACKMOUSEEVENT tme;
				tme.cbSize = sizeof(tme);
				tme.dwFlags = TME_LEAVE;
				tme.hwndTrack = static_cast<Derived*>(this)->GetHwnd();
				::TrackMouseEvent(&tme);
			}
			break;
		case WM_MOUSELEAVE:
			inside = false;
			if(hover != nullptr){
				auto hts = Object::CreateHitTestStruct(static_cast<Derived*>(this));
				hover->OnMouseLeave(hts);
				hover = nullptr;
				SetCursorType(static_cast<CursorType>(static_cast<int>(hts.cursor)));
			}
			break;
		case WM_LBUTTONDOWN:
			{
				auto hts = Object::CreateHitTestStruct(static_cast<Derived*>(this));
				int x = GET_X_LPARAM(lParam);
				int y = GET_Y_LPARAM(lParam);
				bool collide = false;

				pushing = true;

				static_cast<Derived*>(this)->IterateObjects([&](typename Object &o)-> bool{
					if(o.IsColliding(Point(x, y))){
						o.OnLeftPress(Point(x, y), hts);
						if(focus != nullptr && &o != focus){
							focus->OnLoseFocus(hts);
						}
						focus = &o;
						collide = true;
						return true;
					}
					return false;
				});
				if(!collide && focus != nullptr){
					focus->OnLoseFocus(hts);
					focus = nullptr;
				}

				SetCursorType(static_cast<CursorType>(static_cast<int>(hts.cursor)));
				::SetCapture(static_cast<Derived*>(this)->GetHwnd());
			}
			break;
		case WM_LBUTTONUP:
			{
				auto hts = Object::CreateHitTestStruct(static_cast<Derived*>(this));
				int x = GET_X_LPARAM(lParam);
				int y = GET_Y_LPARAM(lParam);

				pushing = false;

				if(focus != nullptr){
					focus->OnLeftRelease(Point(x, y), hts);
				}
				
				SetCursorType(static_cast<CursorType>(static_cast<int>(hts.cursor)));
				::ReleaseCapture();
			}
			break;

		case WM_KILLFOCUS:
			if(focus != nullptr){
				focus->OnLoseFocus(Object::CreateHitTestStruct(static_cast<Derived*>(this)));
				focus = nullptr;
			}

		case WM_SETCURSOR:
			if(inside){
				::SetCursor(hCursor[cursor_type]);
				l = TRUE;
				return false;
			}
		}

		return true;
	}

} }
