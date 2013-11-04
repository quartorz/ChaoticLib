#pragma once

#include "Resource.h"
#include "BaseTypes.h"

namespace ChaoticLib{ namespace Direct2D{

	class Object: public Resource{
	protected:
		Point pos;
		Size size;

	public:
		struct HitTestStruct{
			HWND hwnd;

			// enum class Cursor: int{
			//	Arrow,
			//	Hand,
			//	Ibeam,
			// };
			struct Cursor{
				static const int Arrow = 0;
				static const int Hand = 1;
				static const int Ibeam = 2;

				int cursor;

				Cursor(int c): cursor(c)
				{
				}
				operator int()
				{
					return cursor;
				}
			}cursor;

			void SetCursor(Cursor c)
			{
				cursor = c;
			}
		};
		template <class Window>
		static HitTestStruct CreateHitTestStruct(Window *w)
		{
			HitTestStruct hts = {w->GetHwnd(), HitTestStruct::Cursor::Arrow};
			return hts;
		}

		// 座標はオブジェクトを所有するウィンドウかオブジェクトからの相対座標
		virtual bool IsColliding(const Point &ap)
		{
			return ap.IsInside(GetRect());
		}
		virtual void OnLeftPress(const Point &, HitTestStruct &)
		{
		}
		virtual void OnMouseMove(const Point &, HitTestStruct &)
		{
		}
		virtual void OnLeftRelease(const Point &, HitTestStruct &)
		{
		}
		virtual void OnMouseLeave(const HitTestStruct &)
		{
		}
		virtual void OnLoseFocus(const HitTestStruct &)
		{
		}

		virtual void Draw(const PaintStruct &) = 0;
		virtual Rect GetDrawingRect()
		{
			return Rect(pos, size);
		}
		virtual void SetPosition(const Point &pt)
		{
			pos = pt;
		}
		virtual void SetSize(const Size &s)
		{
			size = s;
		}
		void SetRect(const Rect &r)
		{
			SetPosition(Point(r.left, r.top));
			SetSize(Size(r.Width(), r.Height()));
		}
		virtual Point GetPosition()
		{
			return pos;
		}
		virtual Size GetSize()
		{
			return size;
		}
		Rect GetRect()
		{
			return Rect(GetPosition(), GetSize());
		}
	};

} }
