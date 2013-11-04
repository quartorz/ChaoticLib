#pragma once

#include <d2d1.h>
#include <cmath>

#ifdef max
#undef max
#undef min
#endif

namespace ChaoticLib{ namespace Direct2D{

	class Brush;

	struct PaintStruct{
		ID2D1RenderTarget *target;
		bool debug;
	};

	struct Size: public D2D1_SIZE_F{
		Size(float w=0.f, float h=0.f):
			D2D1_SIZE_F(D2D1::SizeF(w, h))
		{
		}
		template <class T>
		Size(const T &w, const T &h):
			D2D1_SIZE_F(D2D1::SizeF(static_cast<FLOAT>(w), static_cast<FLOAT>(h)))
		{
		}
		Size &operator=(const D2D1_SIZE_U &s)
		{
			width = static_cast<FLOAT>(s.width);
			height = static_cast<FLOAT>(s.height);
			return *this;
		}
		Size &operator=(const D2D1_SIZE_F &s)
		{
			width = s.width;
			height = s.height;
			return *this;
		}
	};
	struct Rect;
	struct Point: public D2D1_POINT_2F{
		Point(float x=0.f, float y=0.f):
			D2D1_POINT_2F(D2D1::Point2F(x, y))
		{
		}
		template <class T>
		Point(const T &x, const T &y):
			D2D1_POINT_2F(D2D1::Point2F(static_cast<FLOAT>(x), static_cast<FLOAT>(y)))
		{
		}
		bool IsInside(const Rect &) const;
	};
	inline Point operator+(Point p1, const Point &p2)
	{
		p1.x += p2.x;
		p1.y += p2.y;
		return p1;
	}
	inline Point operator-(Point p1, const Point &p2)
	{
		p1.x -= p2.x;
		p1.y -= p2.y;
		return p1;
	}
	inline Point operator*(Point p, float f)
	{
		return Point(p.x * f, p.y * f);
	}
	inline Point operator*(float f, Point p)
	{
		return p * f;
	}
	struct Rect: public D2D1_RECT_F{
		Rect(float left=0.f, float top=0.f, float right=0.f, float bottom=0.f):
			D2D1_RECT_F(D2D1::RectF(left, top, right, bottom))
		{
		}
		Rect(const Point &p, const Size &s):
			D2D1_RECT_F(D2D1::RectF(p.x, p.y, p.x + s.width, p.y + s.height))
		{
		}
		float Width() const
		{
			return right - left;
		}
		float Height() const
		{
			return bottom - top;
		}
		bool CollideWith(const Rect &r)
		{
			return left < r.right && right > r.left && top < r.bottom && bottom > r.top;
		}
		void Draw(const PaintStruct &ps, const Brush &brush, float width=1.f);
		void Fill(const PaintStruct &ps, const Brush &brush);
	};

	struct Line{
		Point p1, p2;

	public:
		Line(const Point &p1=Point(), const Point &p2=Point()): p1(p1), p2(p2)
		{
		}
	};

	inline bool Point::IsInside(const Rect &r) const
	{
		return r.left <= x && x <= r.right && r.top <= y && y <= r.bottom;
	}

	inline Rect operator+(Rect r, Point p)
	{
		return Rect(Point(r.left + p.x, r.top + p.y), Size(r.Width(), r.Height()));
	}
	inline Rect operator+(Point p, Rect r)
	{
		return Rect(Point(r.left + p.x, r.top + p.y), Size(r.Width(), r.Height()));
	}

	struct Color: public D2D1::ColorF{
	public:
		Color(Enum e): D2D1::ColorF(e)
		{
		}
		Color(int r, int g, int b, int a=255): D2D1::ColorF(r / 255.f, g / 255.f, b / 255.f, a / 255.f)
		{
		}
		Color(float r=0.f, float g=0.f, float b=0.f, float a=1.f): D2D1::ColorF(r, g, b, a)
		{
		}
		double GetLuminance() const
		{
			return Luminance(r, g, b);
		}
		double GetHue() const
		{
			return Hue(r, g, b);
		}
		double GetSaturation()
		{
			return Saturation(r, g, b);
		}

		static double Luminance(float r, float g, float b)
		{
			return 0.298912 * r + 0.586611 * g + 0.114478 * b;
		}
		static double Hue(float r, float g, float b)
		{
			double u = - r * 0.1471376975169300226 - g * 0.2888623024830699774 + b * 0.436;
			double v =   r * 0.615 - g * 0.514985734664764622 - b * 0.100014265335235378;
			return std::atan2(v, u);
		}
		static double Saturation(float r, float g, float b)
		{
			double u = - r * 0.1471376975169300226 - g * 0.2888623024830699774 + b * 0.436;
			double v =   r * 0.615 - g * 0.514985734664764622 - b * 0.100014265335235378;
			return std::sqrt(u * u + v * v);
		}
		static Color FromHSL(double h, double s, double l, int a)
		{
			double u = cos(h) * s;
			double v = sin(h) * s;
			int r = static_cast<int>((l + 1.139837398373983740  * v) * 255);
			int g = static_cast<int>((l - 0.3946517043589703515  * u - 0.5805986066674976801 * v) * 255);
			int b = static_cast<int>((l + 2.03211091743119266 * u) * 255);
			return Color(r, g, b, a);
		}
	};

} }
