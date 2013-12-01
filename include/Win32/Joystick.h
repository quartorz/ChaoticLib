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
#include <functional>
#include <vector>
#include <tuple>
#include <algorithm>
#include <string>

#include "Procs.h"

namespace ChaoticLib{ namespace Win32{

	template <class Derived, UINT_PTR ID=UINT_PTR_MAX>
	class Joystick{
		using device_type = std::tuple<GUID, LPDIRECTINPUTDEVICE8>;
		std::vector<device_type> devices;

		void LoadDevices(HWND hwnd)
		{
			if(devices.size() != 0)
				return;

			using namespace _com_util;

			_COM_SMARTPTR_TYPEDEF(IDirectInput8, IID_IDirectInput8);
			IDirectInput8Ptr input;
			CheckError(::DirectInput8Create(::GetModuleHandleW(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8, reinterpret_cast<LPVOID*>(&input), nullptr));

			std::vector<std::tuple<GUID, std::wstring>> joysticks;

			LPVOID context[] ={input, &devices, &joysticks};
			input->EnumDevices(DI8DEVCLASS_GAMECTRL, [](const DIDEVICEINSTANCE *pdidInstance, VOID *pContext)->BOOL{
				std::wcerr << L"instance name: " << pdidInstance->tszInstanceName << std::endl;
				std::wcerr << L"product name: " << pdidInstance->tszProductName << std::endl << std::endl;

				auto context = static_cast<LPVOID*>(pContext);
				auto input = static_cast<LPDIRECTINPUT8>(context[0]);
				auto devices = static_cast<std::vector<device_type>*>(context[1]);
				auto joysticks = static_cast<std::vector<std::tuple<GUID, std::wstring>>*>(context[2]);
				LPDIRECTINPUTDEVICE8 device;

				CheckError(input->CreateDevice(
					pdidInstance->guidInstance,
					&device,
					nullptr));

				devices->emplace_back(pdidInstance->guidInstance, device);
				joysticks->emplace_back(pdidInstance->guidInstance, pdidInstance->tszInstanceName);

				return DIENUM_CONTINUE;
			}, context, DIEDFL_ATTACHEDONLY);

			for(auto &tuple: devices){
				LPDIRECTINPUTDEVICE8 device = std::get<1>(tuple);

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

			for(auto &tuple : devices){
				_com_util::CheckError(std::get<1>(tuple)->SetCooperativeLevel(
					hwnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND));
			}

			static_cast<Derived*>(this)->OnReloadJoysticks(joysticks);
		}

		void ReleaseDevices()
		{
			for(auto &tuple: devices){
				SafeRelease(std::get<1>(tuple));
			}
			devices.clear();
		}

	public:
		Joystick()
		{
		}
		~Joystick()
		{
			ReleaseDevices();
		}

		bool HasJoystick() const
		{
			return devices.size() != 0;
		}

		DIDEVCAPS GetJoystickCapabilities(GUID &guid) const
		{
			auto it = std::find_if(devices.begin(), devices.end(), [&guid](device_type &d){return std::get<0>(d) == guid;});
			if(it != devices.end()){
				DIDEVCAPS diDevCaps;
				diDevCaps.dwSize = sizeof(DIDEVCAPS);
				std::get<1>(*it)->GetCapabilities(&diDevCaps);
				return diDevCaps;
			}

			throw std::runtime_error("joystick not found");
		}

		bool WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT &lresult)
		{
			switch(msg){
			case WM_CREATE:
				LoadDevices(hwnd);
				static_cast<Derived*>(this)->AddTimerHandler([this](unsigned id){
					for(auto &tuple : devices){
						auto device = std::get<1>(tuple);

						if(FAILED(device->Poll())){
							while(device->Acquire() == DIERR_INPUTLOST);
							return;
						}

						DIJOYSTATE2 js;
						if(FAILED(device->GetDeviceState(sizeof(DIJOYSTATE2), &js)))
							return;

						static_cast<Derived*>(this)->OnGetJoystickState(std::get<0>(tuple), js);
					}
				}, ID);
				break;
			case WM_DESTROY:
				ReleaseDevices();
				break;
			case WM_DEVICECHANGE:
				ReleaseDevices();
				LoadDevices(hwnd);
				break;
			}

			return true;
		}
	};

} }
