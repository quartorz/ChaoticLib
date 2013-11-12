#pragma once

#include "ChaoticLib\Direct2D\Traits.h"
#include "ChaoticLib\Direct2D\Painter.h"
#include "ChaoticLib\Button.h"
#include "ChaoticLib\FlatButton.h"
#include "ChaoticLib\Scene.h"
#include "ChaoticLib\SceneManager.h"
#include "ChaoticLib\ObjectManager.h"
#include "ChaoticLib\ResourceManager.h"
#include "ChaoticLib\Manager.h"

#include "ChaoticLib\Win32\Window.h"
#include "ChaoticLib\Win32\Procs.h"
#include "ChaoticLib\Win32\Creator.h"
#include "ChaoticLib\Win32\ObjectProcessor.h"

namespace Aliases{
	namespace Drawer = ChaoticLib::Direct2D;

	using Traits = Drawer::Traits;

	using CreateStruct = Traits::CreateStruct;
	using PaintStruct = Traits::PaintStruct;
	using Resource = Traits::Resource;
	using Object = Traits::Object;
	using Color = Traits::Color;
	using Point = Traits::Point;
	using Size = Traits::Size;
	using Rect = Traits::Rect;
	using Line = Traits::Line;
	using Circle = Traits::Circle;
	using Polygon = Traits::Polygon;
	using SolidBrush = Traits::SolidBrush;
	using Font = Traits::Font;
	using Image = Traits::Image;
	using Text = Traits::Text;
	template <class Derived, class Window, bool CheckDuplicate=false, bool MultiThread=false>
	using UserDefinedObject = Traits::UserDefinedObject<Derived, Window, CheckDuplicate, MultiThread>;

	using Button = ChaoticLib::Button<Traits>;
	using FlatButton = ChaoticLib::FlatButton<Traits>;

	template <class Window>
	using Scene = ChaoticLib::Scene<Window, Traits>;

	template <class Derived>
	using SceneManager = ChaoticLib::SceneManager<Derived, Traits>;

	template <class Derived, bool CheckDuplicate=false, bool MultiThread=false>
	using ObjectManager = ChaoticLib::ObjectManager<Derived, Traits, CheckDuplicate, MultiThread>;

	template <class Derived, bool CheckDuplicate=false, bool MultiThread=false>
	using ResourceManager = ChaoticLib::ResourceManager<Derived, Traits, CheckDuplicate, MultiThread>;

	template <class Derived, bool CheckDuplicate=false, bool MultiThread=false>
	using Manager = ChaoticLib::Manager<Derived, Traits, CheckDuplicate, MultiThread>;

	template <class Derived>
	using Painter = Drawer::Painter<Derived>;

	namespace WindowSystem = ChaoticLib::Win32;

	template <class Derived>
	using ObjectProcessor = WindowSystem::ObjectProcessor<Derived, Traits>;
}
