#pragma once

#include "BaseTypes.h"
#include "Brush.h"

namespace ChaoticLib{ namespace Direct2D{
	class SolidBrush: public Brush{
		Color color;
	public:
		SolidBrush(const Color &c=Color())
			:color(c)
		{
		}
		SolidBrush(const SolidBrush &b)
			:color(b.color)
		{
		}

		void SetColor(const Color &);

		virtual bool CreateResource(const CreateStruct &) override;
		virtual void DestroyResource() override;

		const Color &GetColor() const
		{
			return color;
		}
	};
} }
