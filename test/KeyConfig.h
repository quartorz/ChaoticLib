#pragma once

#include <Windows.h>
#include <dinput.h>

#include <memory>

class KeyConfigClass sealed{
public:
	enum class Type{
		Keyboard,
		Joystick,
	};
	enum class Button{
		Up,
		Down,
		Left,
		Right,
		Select,
		Cancel,
		Menu,
	};
	struct State{
		bool up, down, left, right, select, cancel, menu;
	};

private:
	struct JoystickButton{
		enum class Tag{
			Axis,
			Rot,
			Slider,
			POV,
			Button,
		}tag;

		int n;
		bool positive;
	};

	static std::shared_ptr<KeyConfigClass> instance;

	Type type;
	GUID js_id;
	JoystickButton js_config[7];
	unsigned kb_config[7];
	State state[2];

	KeyConfigClass() = default;

public:
	KeyConfigClass(const KeyConfigClass&) = delete;
	KeyConfigClass &operator=(const KeyConfigClass&) = delete;

	static std::shared_ptr<KeyConfigClass> &GetInstance()
	{
		if(!instance){
			instance = std::make_shared<KeyConfigClass>();
		}
		return instance;
	}

	State GetState() const;
	void SetType(Type);
	void SetKeyboardConfig(Button, unsigned);
	void SetJoystickConfig(Button, const GUID &, const DIJOYSTATE2 &);
	void SetKeyboardState(unsigned, bool push);
	void SetJoystickState(const GUID &, const DIJOYSTATE2 &);
};

using KeyConfig = std::shared_ptr<KeyConfigClass>;
