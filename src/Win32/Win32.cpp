#include <Windows.h>

namespace ChaoticLib{ namespace Win32{

	LARGE_INTEGER freq;

	bool Initialize()
	{
		::QueryPerformanceFrequency(&freq);

		return true;
	}

	void Uninitialize()
	{
	}

} }