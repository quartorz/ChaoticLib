#pragma once

#include "BaseTypes.h"
#include "Resource.h"
#include "Object.h"
#include "Brush.h"
#include "SolidBrush.h"
#include "Font.h"
#include "Image.h"
#include "Text.h"
#include "UserDefinedObject.h"

namespace ChaoticLib{ namespace Direct2D{

	struct Traits{
		typedef CreateStruct CreateStruct;
		typedef PaintStruct PaintStruct;
		typedef Color Color;
		typedef Point Point;
		typedef Size Size;
		typedef Rect Rect;
		typedef Line Line;
		typedef Circle Circle;
		typedef Polygon Polygon;
		typedef Resource Resource;
		typedef Object Object;
		typedef Brush Brush;
		typedef SolidBrush SolidBrush;
		typedef Font Font;
		typedef Image Image;
		typedef Text Text;

#ifndef DONT_USE_TEMPLATE_ALIASES
		template <class Derived, class Window, bool CheckDuplicate=false, bool MultiThread=false>
		using UserDefinedObject = UserDefinedObject<Derived, Window, CheckDuplicate, MultiThread>;
#else
		template <class Derived, class Window, bool CheckDuplicate=false, bool MultiThread=false>
		struct UserDefinedObject{
			typedef ChaoticLib::Direct2D::UserDefinedObject<Derived, Window, CheckDuplicate, MultiThread> type;
		};
#endif
	};

} }
