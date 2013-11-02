#pragma once

#include "Resource.h"
#include "..\Macro.h"

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

} }
