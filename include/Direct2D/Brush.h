#pragma once

#include "Resource.h"
#include "..\Macro.h"

#include "BaseTypes.h"

namespace ChaoticLib{ namespace Direct2D{

	class Brush: public Resource{
	protected:
		ID2D1Brush *brush;

	public:
		Brush(): brush(nullptr)
		{
		}
		virtual ~Brush()
		{
			SafeRelease(brush);
		}

		ID2D1Brush *Get() const
		{
			return brush;
		}
	};


	inline void Rect::Draw(const PaintStruct &ps, const Brush &brush, float width /* =1.f */)
	{
		ps.target->DrawRectangle(
			*this,
			brush.Get(),
			width);
	}

	inline void Rect::Fill(const PaintStruct &ps, const Brush &brush)
	{
		ps.target->FillRectangle(
			*this,
			brush.Get());
	}

	inline void Line::Draw(const PaintStruct &ps, const Brush &brush, float width /* =1.f */)
	{
		ps.target->DrawLine(
			p1,
			p2,
			brush.Get(),
			width);
	}

	inline void Circle::Draw(const PaintStruct &ps, const Brush &brush, float width /* =1.f */)
	{
		ps.target->DrawEllipse(
			D2D1::Ellipse(center, radius, radius),
			brush.Get(),
			width);
	}

	inline void Circle::Fill(const PaintStruct &ps, const Brush &brush)
	{
		ps.target->FillEllipse(
			D2D1::Ellipse(center, radius, radius),
			brush.Get());
	}

	inline void Polygon::Draw(const PaintStruct &ps, const Brush &brush, float width /* =1.f */)
	{
		Point p = GetPosition();

		D2D1::Matrix3x2F transform;

		ps.target->GetTransform(&transform);
		ps.target->SetTransform(transform * D2D1::Matrix3x2F::Translation(p.x, p.y));

		ps.target->DrawGeometry(
			geometry,
			brush.Get(),
			width);

		ps.target->SetTransform(transform);
	}

	inline void Polygon::Fill(const PaintStruct &ps, const Brush &brush)
	{
		Point p = GetPosition();

		D2D1::Matrix3x2F transform;

		ps.target->GetTransform(&transform);
		ps.target->SetTransform(transform * D2D1::Matrix3x2F::Translation(p.x, p.y));

		ps.target->FillGeometry(
			geometry,
			brush.Get());

		ps.target->SetTransform(transform);
	}

} }
