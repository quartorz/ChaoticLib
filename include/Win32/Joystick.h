#pragma once

#if !defined DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif

#include <dinput.h>
#include <comdef.h>

#include "..\Macro.h"

#pragma comment(lib, "Dinput8")
#pragma comment(lib, "dxguid")

#include <iostream>
#include <cstdint>
#include <functional>
#include <random>
#include <ctime>
#include <vector>
#include <tuple>
#include <algorithm>

namespace ChaoticLib{ namespace Win32{

	template <class Derived, UINT Time=100, UINT_PTR ID=UINT_PTR_MAX>
	class Joystick{
		LPDIRECTINPUTDEVICE8 device;

		using handler_type = std::function<void(DIJOYSTATE2&)>;
		using hash_type = std::mt19937_64::result_type;
		using tuple_type = std::tuple<hash_type, handler_type>;
		using container_type = std::vector<tuple_type>;
		container_type handlers;

		std::mt19937_64 rand{std::time(nullptr)};

	public:
		Joystick(): device(nullptr)
		{
			using namespace _com_util;

			_COM_SMARTPTR_TYPEDEF(IDirectInput8, IID_IDirectInput8);
			IDirectInput8Ptr input;
			CheckError(::DirectInput8Create(::GetModuleHandleW(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8, reinterpret_cast<LPVOID*>(&input), nullptr));

			LPVOID context[] ={input, &device};
			input->EnumDevices(DI8DEVCLASS_GAMECTRL, [](const DIDEVICEINSTANCE *pdidInstance, VOID *pContext)->BOOL{
				std::wcout << L"instance name: " << pdidInstance->tszInstanceName << std::endl;
				std::wcout << L"product name: " << pdidInstance->tszProductName << std::endl << std::endl;

				auto context = static_cast<LPVOID*>(pContext);
				auto input = static_cast<LPDIRECTINPUT8>(context[0]);
				auto device = static_cast<LPDIRECTINPUTDEVICE8*>(context[1]);

				CheckError(input->CreateDevice(
					pdidInstance->guidInstance,
					device,
					nullptr));

				return DIENUM_STOP;
			}, context, DIEDFL_ATTACHEDONLY);

			if(device == nullptr)
				return;

			CheckError(device->SetDataFormat(&c_dfDIJoystick2));

			device->EnumObjects([](LPCDIDEVICEOBJECTINSTANCE pdidoi, LPVOID pvRef)->BOOL{
				auto device = static_cast<LPDIRECTINPUTDEVICE>(pvRef);

				DIPROPRANGE diprg;

				diprg.diph.dwSize = sizeof(DIPROPRANGE);
				diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER);
				diprg.diph.dwHow = DIPH_BYID;
				diprg.diph.dwObj = pdidoi->dwType;
				diprg.lMin = -32768;
				diprg.lMax = 32767;

				if(FAILED(device->SetProperty(DIPROP_RANGE, &diprg.diph)))
					return DIENUM_STOP;

				return DIENUM_CONTINUE;
			}, device, DIDFT_AXIS);
		}
		~Joystick()
		{
			SafeRelease(device);
		}

		bool HasJoystick() const
		{
			return device != nullptr;
		}

		DIDEVCAPS GetJoystickCapabilities() const
		{
			DIDEVCAPS diDevCaps;
			diDevCaps.dwSize = sizeof(DIDEVCAPS);
			device->GetCapabilities(&diDevCaps);
			return diDevCaps;
		}

		hash_type AddJoystickHandler(const std::function<void(DIJOYSTATE2&)> &func)
		{
			auto hash = rand();
			handlers.emplace_back(hash, func);
			return hash;
		}

		void DeleteJoystickHandler(hash_type hash)
		{
			std::remove_if(handlers.begin(), handlers.end(), [&hash](container_type::value_type &v){return std::get<0>(v) == hash;});
		}

		bool WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT &lresult)
		{
			if(HasJoystick()){
				if(msg == WM_CREATE){
					_com_util::CheckError(device->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND));
					::SetTimer(hwnd, ID, Time, nullptr);
				}else if(msg == WM_TIMER && wParam == ID){
					if(FAILED(device->Poll())){
						while(device->Acquire() == DIERR_INPUTLOST);
						return true;
					}

					DIJOYSTATE2 js;
					if(FAILED(device->GetDeviceState(sizeof(DIJOYSTATE2), &js)))
						return true;

					for(auto &tuple: handlers){
						std::get<1>(tuple)(js);
					}
				}
			}

			return true;
		}
	};

} }
