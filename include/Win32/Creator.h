#pragma once

#include <Windows.h>

namespace ChaoticLib{ namespace Win32{

	template <class Derived>
	class Creator{
	public:
		bool Create(
			DWORD exstyle=0,
			DWORD style=WS_OVERLAPPEDWINDOW,
			const wchar_t *title=L"",
			int x=CW_USEDEFAULT,
			int y=0,
			int w=CW_USEDEFAULT,
			int h=0,
			HWND hparent=nullptr)
		{
			HWND hwnd = ::CreateWindowExW(
				exstyle,
				static_cast<Derived*>(this)->GetClassName(),
				title,
				style,
				x,
				y,
				w,
				h,
				hparent,
				nullptr,
				::GetModuleHandleW(NULL),
				static_cast<Derived*>(this));
			return hwnd != nullptr;
		}
	};

} }
