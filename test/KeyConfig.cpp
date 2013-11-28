#include "KeyConfig.h"
#include "StringTable.h"

#include "DataParser.h"
#include "Function.h"

#include <algorithm>
#include <fstream>
#include <iomanip>

#pragma comment(lib, "rpcrt4")

#pragma warning(disable:4351)

namespace{
	int axis_threshold = 15000;

}

std::unique_ptr<KeyConfig> KeyConfig::instance;

KeyConfig::KeyConfig():
	js_id(),
	js_config(),
	kb_config(),
	state()
{
	auto path = GetDirectory() + L"keyconfig.dat";

	std::wifstream ifs(path, std::ios_base::in);
	if(ifs){
		Data<wchar_t> data;
		try{
			data = Data<wchar_t>::parse<true>(ifs);
		}catch(std::runtime_error &){
			return;
		}

		int i = 0;
		for(auto &key : data.map()[L"keyboard"].vector()){
			auto it = std::find(std::begin(::kb_table), std::end(::kb_table), key.string());
			auto dist = std::distance(std::begin(::kb_table), it);
			kb_config[i] = dist;
			i++;
		}
		i = 0;
		for(auto &key : data.map()[L"joystick"].map()[L"buttons"].vector()){
			auto it = std::find(std::begin(::js_table), std::end(::js_table), key.string());
			auto dist = std::distance(std::begin(::js_table), it);
			js_config[i].tag = static_cast<JoystickButton::Tag>(dist);
			i++;
		}
		auto &id = data.map()[L"joystick"].map()[L"id"].string();
		::UuidFromStringW(reinterpret_cast<RPC_WSTR>(const_cast<wchar_t*>(id.c_str())), &js_id);
	}
}

KeyConfig::~KeyConfig()
{
	wchar_t path[_MAX_PATH];
	::GetModuleFileNameW(nullptr, path, _MAX_PATH);
	::wcscpy_s(::wcsrchr(path, L'\\') + 1, 14, L"keyconfig.dat");

	int i = 0;

	std::wofstream ofs(path, std::ios_base::out);
	ofs << L"{\n"
		 LR"(  "keyboard": [)"  L"\n";
	for(auto key: kb_config){
		ofs << LR"(    ")" << ToString(key) << LR"(")";
		if(i != 7)
			ofs << L",";
		ofs << L"\n";
		i++;
	}
	ofs << L"  ],\n"
		 LR"(  "joystick": {)" L"\n"
		 LR"(    "buttons": [)" L"\n";
	i = 0;
	for(auto key : js_config){
		ofs << LR"(      ")" << ToString(key.tag) << LR"(")";
		if(i != 7)
			ofs << L",";
		ofs << L"\n";
		i++;
	}
	ofs << L"    ],\n"
		 LR"(    "id": ")";
	ofs << std::hex << std::setw(8) << std::setfill(L'0') << js_id.Data1 << L'-';
	ofs << std::hex << std::setw(4) << std::setfill(L'0') << js_id.Data2 << L'-';
	ofs << std::hex << std::setw(4) << std::setfill(L'0') << js_id.Data3 << L'-';
	ofs << std::hex << std::setw(2) << std::setfill(L'0') << js_id.Data4[0];
	ofs << std::hex << std::setw(2) << std::setfill(L'0') << js_id.Data4[1] << L'-';
	ofs << std::hex << std::setw(2) << std::setfill(L'0') << js_id.Data4[2];
	ofs << std::hex << std::setw(2) << std::setfill(L'0') << js_id.Data4[3];
	ofs << std::hex << std::setw(2) << std::setfill(L'0') << js_id.Data4[4];
	ofs << std::hex << std::setw(2) << std::setfill(L'0') << js_id.Data4[5];
	ofs << std::hex << std::setw(2) << std::setfill(L'0') << js_id.Data4[6];
	ofs << std::hex << std::setw(2) << std::setfill(L'0') << js_id.Data4[7] << L"\"\n";
	ofs << L"  }\n}";
}

const wchar_t *KeyConfig::ToString(JoystickButton::Tag tag)
{
	return ::js_table[static_cast<int>(tag)];
}

const wchar_t *KeyConfig::ToString(unsigned keycode)
{
	return ::kb_table[keycode];
}

KeyConfig::State KeyConfig::GetState() const
{
	return {
		state[0] != 0,
		state[1] != 0,
		state[2] != 0,
		state[3] != 0,
		state[4] != 0,
		state[5] != 0,
		state[6] != 0,
		state[7] != 0,
	};
}

void KeyConfig::Reset()
{
	std::fill(std::begin(state), std::end(state), 0);
}

const wchar_t *KeyConfig::GetKeyboardConfig(Button button) const
{
	return ::kb_table[kb_config[static_cast<int>(button)]];
}

const wchar_t *KeyConfig::GetJoystickConfig(Button button) const
{
	return ::js_table[static_cast<int>(js_config[static_cast<int>(button)].tag)];
}

const wchar_t *KeyConfig::SetKeyboardConfig(Button button, unsigned keycode)
{
	kb_config[static_cast<int>(button)] = keycode;
	return ::kb_table[keycode];
}

const wchar_t *KeyConfig::SetJoystickConfig(Button button, const GUID &id, const DIJOYSTATE2 &js)
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
				return ToString(JoystickButton::Tag::None);
			}
		}
	}
	return ToString(config.tag);
}

void KeyConfig::SetKeyboardState(unsigned keycode, bool push)
{
	auto it = std::find(std::begin(kb_config), std::end(kb_config), keycode);
	if(it != std::end(kb_config)){
		auto dist = std::distance(std::begin(kb_config), it);
		if(push)
			state[dist] |= 0x10;
		else
			state[dist] &= ~0x10;
	}
}

void KeyConfig::SetJoystickState(const GUID &id, const DIJOYSTATE2 &js)
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
