#pragma once

#include <Windows.h>
#include <dinput.h>

#include <memory>
#include <numeric>

class KeyConfig sealed{
public:
	enum class Button{
		Up,
		Down,
		Left,
		Right,
		A,
		B,
		C,
		Menu,
	};
	struct State{
		bool up, down, left, right, a, b, c, menu;
	};

private:
	struct JoystickButton{
		enum class Tag{
			None,
			Axis0_Positive,
			Axis0_Negative,
			Axis1_Positive,
			Axis1_Negative,
			Axis2_Positive,
			Axis2_Negative,
			Rot0_Positive,
			Rot0_Negative,
			Rot1_Positive,
			Rot1_Negative,
			Rot2_Positive,
			Rot2_Negative,
			Slider0_Positive,
			Slider0_Negative,
			Slider1_Positive,
			Slider1_Negative,
			POV0_Up,
			POV0_Down,
			POV0_Left,
			POV0_Right,
			POV1_Up,
			POV1_Down,
			POV1_Left,
			POV1_Right,
			POV2_Up,
			POV2_Down,
			POV2_Left,
			POV2_Right,
			POV3_Up,
			POV3_Down,
			POV3_Left,
			POV3_Right,
			Button0,
		}tag;
	};
	const wchar_t *ToString(JoystickButton::Tag);
	const wchar_t *ToString(unsigned);

	static std::unique_ptr<KeyConfig> instance;

	GUID js_id;
	JoystickButton js_config[8];
	unsigned kb_config[8];
	unsigned char state[8];

	KeyConfig();

public:
	KeyConfig(const KeyConfig&) = delete;
	KeyConfig &operator=(const KeyConfig&) = delete;

	~KeyConfig();

	static KeyConfig *GetInstance()
	{
		if(!instance){
			instance = std::unique_ptr<KeyConfig>(new KeyConfig);
		}
		return instance.get();
	}

	State GetState() const;
	void Reset();
	const GUID &GetJoystickID() const
	{
		return js_id;
	}
	const wchar_t *GetKeyboardConfig(Button) const;
	const wchar_t *GetJoystickConfig(Button) const;
	const wchar_t *SetKeyboardConfig(Button, unsigned);
	const wchar_t *SetJoystickConfig(Button, const GUID &, const DIJOYSTATE2 &);
	void SetKeyboardState(unsigned, bool push);
	void SetJoystickState(const GUID &, const DIJOYSTATE2 &);
};
