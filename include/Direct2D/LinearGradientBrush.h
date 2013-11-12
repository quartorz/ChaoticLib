#pragma once

#include "Object.h"
#include "BaseTypes.h"

#include <vector>

#include <d2d1.h>

namespace ChaoticLib{ namespace Direct2D{

	class LinearGradientBrush: Object{
		ID2D1PathGeometry *geometry;
		std::vector<Point> points;

	public:

		void AddPoint(const Point &p);

		virtual bool CreateResource(const CreateStruct &) override;
		virtual void DestroyResource() override;
		virtual void Draw(const PaintStruct &) override;
	};

} }
