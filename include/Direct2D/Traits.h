#pragma once

#include "Resource.h"
#include "Brush.h"
#include "SolidBrush.h"
#include "BaseTypes.h"

namespace ChaoticLib{ namespace Direct2D{

	struct Traits{
		typedef Resource Resource;
		typedef Color Color;
		typedef Brush Brush;
		typedef SolidBrush SolidBrush;
	};

} }
