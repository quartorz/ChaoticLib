#pragma once

#include "BaseTypes.h"
#include "Resource.h"
#include "Object.h"
#include "Brush.h"
#include "SolidBrush.h"
#include "Font.h"
#include "Image.h"
#include "Text.h"

namespace ChaoticLib{ namespace Direct2D{

	struct Traits{
		typedef PaintStruct PaintStruct;
		typedef Color Color;
		typedef Point Point;
		typedef Size Size;
		typedef Rect Rect;
		typedef Resource Resource;
		typedef Object Object;
		typedef SolidBrush SolidBrush;
		typedef Font Font;
		typedef Image Image;
		typedef Text Text;
	};

} }
