#include "KeyConfig.h"

namespace{
	int axis_threshold = 15000;
}

KeyConfig KeyConfigClass::instance;

KeyConfigClass::State KeyConfigClass::GetState() const
{
	if(type == Type::Keyboard)
		return state[0];
	else
		return state[1];
}

void KeyConfigClass::SetType(Type t)
{
	type = t;
}

void KeyConfigClass::SetKeyboardConfig(Button button, unsigned keycode)
{
	kb_config[static_cast<int>(button)] = keycode;
}

void KeyConfigClass::SetJoystickConfig(Button button, const GUID &id, const DIJOYSTATE2 &js)
{
	js_id = id;

	auto config = js_config[static_cast<int>(button)];
	if(js.lX > axis_threshold){
		config.tag = JoystickButton::Tag::Axis;
		config.n = 0;
		config.positive = true;
	}else if(js.lX < axis_threshold){
		config.tag = JoystickButton::Tag::Axis;
		config.n = 0;
		config.positive = false;
	}else if(js.lY > axis_threshold){
		config.tag = JoystickButton::Tag::Axis;
		config.n = 1;
		config.positive = true;
	}else if(js.lY < axis_threshold){
		config.tag = JoystickButton::Tag::Axis;
		config.n = 1;
		config.positive = false;
	}else if(js.lZ > axis_threshold){
		config.tag = JoystickButton::Tag::Axis;
		config.n = 2;
		config.positive = true;
	}else if(js.lZ < axis_threshold){
		config.tag = JoystickButton::Tag::Axis;
		config.n = 2;
		config.positive = false;
	}else if(js.lRx > axis_threshold){
		config.tag = JoystickButton::Tag::Rot;
		config.n = 0;
		config.positive = true;
	}else if(js.lRx < axis_threshold){
		config.tag = JoystickButton::Tag::Rot;
		config.n = 0;
		config.positive = false;
	}else if(js.lRy > axis_threshold){
		config.tag = JoystickButton::Tag::Rot;
		config.n = 1;
		config.positive = true;
	}else if(js.lRy < axis_threshold){
		config.tag = JoystickButton::Tag::Rot;
		config.n = 1;
		config.positive = false;
	}else if(js.lRz > axis_threshold){
		config.tag = JoystickButton::Tag::Rot;
		config.n = 2;
		config.positive = true;
	}else if(js.lRz < axis_threshold){
		config.tag = JoystickButton::Tag::Rot;
		config.n = 2;
		config.positive = false;
	}else if(js.rglSlider[0] > axis_threshold){
		config.tag = JoystickButton::Tag::Slider;
		config.n = 0;
		config.positive = true;
	}else if(js.rglSlider[0] < axis_threshold){
		config.tag = JoystickButton::Tag::Slider;
		config.n = 0;
		config.positive = false;
	}else if(js.rglSlider[1] > axis_threshold){
		config.tag = JoystickButton::Tag::Slider;
		config.n = 1;
		config.positive = true;
	}else if(js.rglSlider[1] < axis_threshold){
		config.tag = JoystickButton::Tag::Slider;
		config.n = 1;
		config.positive = false;
	}else if(js.rgdwPOV[0] > axis_threshold){
		config.tag = JoystickButton::Tag::POV;
		config.n = 0;
		config.positive = true;
	}else if(js.rglSlider[0] < axis_threshold){
		config.tag = JoystickButton::Tag::Slider;
		config.n = 0;
		config.positive = false;
	}
}

void KeyConfigClass::SetKeyboardState(unsigned, bool)
{
}

void KeyConfigClass::SetJoystickState(const GUID &, const DIJOYSTATE2 &)
{
}
