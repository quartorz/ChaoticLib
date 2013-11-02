#pragma once

#include "BaseTypes.h"
#include "Brush.h"

namespace ChaoticLib{ namespace Direct2D{
	class SolidBrush: public Brush{
		Color color;
	public:
		SolidBrush(const Color &c)
			:color(c)
		{
		}
		SolidBrush(const SolidBrush &b)
			:color(b.color)
		{
		}

		void SetColor(const Color &);

		virtual bool CreateResource(const CreateStruct &);
		virtual void DestroyResource();

		const Color &GetColor() const
		{
			return color;
		}
	};
} }
