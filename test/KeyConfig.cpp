#include "KeyConfig.h"
#include "StringTable.h"

#include <algorithm>

namespace{
	int axis_threshold = 15000;

}

KeyConfig KeyConfigClass::instance;

const wchar_t *KeyConfigClass::ToString(JoystickButton::Tag tag)
{
	return ::js_table[static_cast<int>(tag)];
}

const wchar_t *KeyConfigClass::ToString(unsigned keycode)
{
	return ::kb_table[keycode];
}

KeyConfigClass::State KeyConfigClass::GetState() const
{
	return {
		state[0] != 0,
		state[1] != 0,
		state[2] != 0,
		state[3] != 0,
		state[4] != 0,
		state[5] != 0,
		state[6] != 0,
	};
}

void KeyConfigClass::Reset()
{
	std::fill(std::begin(state), std::end(state), 0);
}

const wchar_t *KeyConfigClass::GetKeyboardConfig(Button button) const
{
	return ::kb_table[kb_config[static_cast<int>(button)]];
}

const wchar_t *KeyConfigClass::GetJoystickConfig(Button button) const
{
	return ::js_table[static_cast<int>(js_config[static_cast<int>(button)].tag)];
}

const wchar_t *KeyConfigClass::SetKeyboardConfig(Button button, unsigned keycode)
{
	kb_config[static_cast<int>(button)] = keycode;
	return ::kb_table[keycode];
}

const wchar_t *KeyConfigClass::SetJoystickConfig(Button button, const GUID &id, const DIJOYSTATE2 &js)
{
	js_id = id;

	auto &config = js_config[static_cast<int>(button)];
	if(js.lX > axis_threshold){
		config.tag = JoystickButton::Tag::Axis0_Positive;
	}else if(js.lX < - axis_threshold){
		config.tag = JoystickButton::Tag::Axis0_Negative;
	}else if(js.lY > axis_threshold){
		config.tag = JoystickButton::Tag::Axis1_Positive;
	}else if(js.lY < -axis_threshold){
		config.tag = JoystickButton::Tag::Axis1_Negative;
	}else if(js.lZ > axis_threshold){
		config.tag = JoystickButton::Tag::Axis2_Positive;
	}else if(js.lZ < -axis_threshold){
		config.tag = JoystickButton::Tag::Axis2_Negative;
	}else if(js.lRx > axis_threshold){
		config.tag = JoystickButton::Tag::Rot0_Positive;
	}else if(js.lRx < - axis_threshold){
		config.tag = JoystickButton::Tag::Rot0_Negative;
	}else if(js.lRy > axis_threshold){
		config.tag = JoystickButton::Tag::Rot1_Positive;
	}else if(js.lRy < - axis_threshold){
		config.tag = JoystickButton::Tag::Rot1_Negative;
	}else if(js.lRz > axis_threshold){
		config.tag = JoystickButton::Tag::Rot2_Positive;
	}else if(js.lRz < - axis_threshold){
		config.tag = JoystickButton::Tag::Rot2_Negative;
	}else if(js.rglSlider[0] > axis_threshold){
		config.tag = JoystickButton::Tag::Slider0_Positive;
	}else if(js.rglSlider[0] < - axis_threshold){
		config.tag = JoystickButton::Tag::Slider0_Negative;
	}else if(js.rglSlider[1] > axis_threshold){
		config.tag = JoystickButton::Tag::Slider1_Positive;
	}else if(js.rglSlider[1] < - axis_threshold){
		config.tag = JoystickButton::Tag::Slider1_Negative;
	}else{
		auto SetPOV = [](DWORD pov, JoystickButton &config, JoystickButton::Tag tag)->bool{
			int pov_n = (static_cast<int>(tag) - static_cast<int>(JoystickButton::Tag::POV0_Up)) / 4;
			if(LOWORD(pov) == 0xFFFF)
				return false;
			if(pov >= 31500 || pov <= 4500){
				config.tag = tag;
			}else if(13500 <= pov && pov <= 22500){
				config.tag = static_cast<JoystickButton::Tag>(static_cast<int>(tag)+1);
			}else if(22500 <= pov && pov <= 31500){
				config.tag = static_cast<JoystickButton::Tag>(static_cast<int>(tag)+2);
			}else if(4500 <= pov && pov <= 13500){
				config.tag = static_cast<JoystickButton::Tag>(static_cast<int>(tag)+3);
			}else
				return false;
			return true;
		};

		if(!SetPOV(js.rgdwPOV[0], config, JoystickButton::Tag::POV0_Up) && !SetPOV(js.rgdwPOV[1], config, JoystickButton::Tag::POV1_Up)
			&& !SetPOV(js.rgdwPOV[2], config, JoystickButton::Tag::POV2_Up) && !SetPOV(js.rgdwPOV[3], config, JoystickButton::Tag::POV3_Up))
		{
			bool found = false;
			for(unsigned i = 0; i < 128; ++i){
				if((js.rgbButtons[i] & 0x80) != 0){
					config.tag = static_cast<JoystickButton::Tag>(static_cast<int>(JoystickButton::Tag::Button0) + i);
					found = true;
					break;
				}
			}
			if(!found){
				config.tag = JoystickButton::Tag::None;
			}
		}
	}
	return ToString(config.tag);
}

void KeyConfigClass::SetKeyboardState(unsigned keycode, bool push)
{
}

void KeyConfigClass::SetJoystickState(const GUID &id, const DIJOYSTATE2 &js)
{
	if(js_id != id)
		return;

	using JoystickButton = JoystickButton;

	auto IsPushing = [](JoystickButton &button, const DIJOYSTATE2 &js)->bool{
		switch(button.tag){
		case JoystickButton::Tag::None:
			return false;
		case JoystickButton::Tag::Axis0_Positive:
			return js.lX > axis_threshold;
		case JoystickButton::Tag::Axis0_Negative:
			return js.lX < - axis_threshold;
		case JoystickButton::Tag::Axis1_Positive:
			return js.lY > axis_threshold;
		case JoystickButton::Tag::Axis1_Negative:
			return js.lY < - axis_threshold;
		case JoystickButton::Tag::Axis2_Positive:
			return js.lZ > axis_threshold;
		case JoystickButton::Tag::Axis2_Negative:
			return js.lZ < - axis_threshold;
		case JoystickButton::Tag::Rot0_Positive:
			return js.lRx > axis_threshold;
		case JoystickButton::Tag::Rot0_Negative:
			return js.lRx < - axis_threshold;
		case JoystickButton::Tag::Rot1_Positive:
			return js.lRy > axis_threshold;
		case JoystickButton::Tag::Rot1_Negative:
			return js.lRy < - axis_threshold;
		case JoystickButton::Tag::Rot2_Positive:
			return js.lRz > axis_threshold;
		case JoystickButton::Tag::Rot2_Negative:
			return js.lRz < - axis_threshold;
		case JoystickButton::Tag::Slider0_Positive:
			return js.rglSlider[0] > axis_threshold;
		case JoystickButton::Tag::Slider0_Negative:
			return js.rglSlider[0] < - axis_threshold;
		case JoystickButton::Tag::Slider1_Positive:
			return js.rglSlider[1] > axis_threshold;
		case JoystickButton::Tag::Slider1_Negative:
			return js.rglSlider[1] < - axis_threshold;
		case JoystickButton::Tag::POV0_Up:
			return LOWORD(js.rgdwPOV[0]) != 0xFFFF && (js.rgdwPOV[0] >= 31500 || js.rgdwPOV[0] <= 4500);
		case JoystickButton::Tag::POV0_Down:
			return 13500 <= js.rgdwPOV[0] && js.rgdwPOV[0] <= 22500;
		case JoystickButton::Tag::POV0_Left:
			return 22500 <= js.rgdwPOV[0] && js.rgdwPOV[0] <= 31500;
		case JoystickButton::Tag::POV0_Right:
			return 4500 <= js.rgdwPOV[0] && js.rgdwPOV[0] <= 13500;
		case JoystickButton::Tag::POV1_Up:
			return LOWORD(js.rgdwPOV[1]) != 0xFFFF && (js.rgdwPOV[1] >= 31500 || js.rgdwPOV[1] <= 4500);
		case JoystickButton::Tag::POV1_Down:
			return 13500 <= js.rgdwPOV[1] && js.rgdwPOV[1] <= 22500;
		case JoystickButton::Tag::POV1_Left:
			return 22500 <= js.rgdwPOV[1] && js.rgdwPOV[1] <= 31500;
		case JoystickButton::Tag::POV1_Right:
			return 4500 <= js.rgdwPOV[1] && js.rgdwPOV[1] <= 13500;
		case JoystickButton::Tag::POV2_Up:
			return LOWORD(js.rgdwPOV[2]) != 0xFFFF && (js.rgdwPOV[2] >= 31500 || js.rgdwPOV[2] <= 4500);
		case JoystickButton::Tag::POV2_Down:
			return 13500 <= js.rgdwPOV[2] && js.rgdwPOV[2] <= 22500;
		case JoystickButton::Tag::POV2_Left:
			return 22500 <= js.rgdwPOV[2] && js.rgdwPOV[2] <= 31500;
		case JoystickButton::Tag::POV2_Right:
			return 4500 <= js.rgdwPOV[2] && js.rgdwPOV[2] <= 13500;
		case JoystickButton::Tag::POV3_Up:
			return LOWORD(js.rgdwPOV[3]) != 0xFFFF && (js.rgdwPOV[3] >= 31500 || js.rgdwPOV[3] <= 4500);
		case JoystickButton::Tag::POV3_Down:
			return 13500 <= js.rgdwPOV[3] && js.rgdwPOV[3] <= 22500;
		case JoystickButton::Tag::POV3_Left:
			return 22500 <= js.rgdwPOV[3] && js.rgdwPOV[3] <= 31500;
		case JoystickButton::Tag::POV3_Right:
			return 4500 <= js.rgdwPOV[3] && js.rgdwPOV[3] <= 13500;
		default:
			return (js.rgbButtons[static_cast<int>(button.tag) - static_cast<int>(JoystickButton::Tag::Button0)] & 0x80) != 0;
		}
		__assume(0);
	};

	std::transform(std::begin(js_config), std::end(js_config), std::begin(state), std::begin(state), [&js, IsPushing](JoystickButton &b, unsigned char s)->unsigned char{
		if(IsPushing(b, js))
			return s | 0x01;
		else
			return s & ~0x01;
	});
}
