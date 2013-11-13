#include "ChaoticLib\ChaoticLib.h"
#include "Joystick.h"

#include <cstdio>

int Run()
{
	MainWindow::Register();

	RECT rc = {0, 0, 800, 600};
	::AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW & ~WS_SIZEBOX, FALSE);

	MainWindow w;
	w.Create(0, WS_OVERLAPPEDWINDOW & ~WS_SIZEBOX, L"Sample", CW_USEDEFAULT, 0, rc.right - rc.left, rc.bottom - rc.top);
	w.Show();

//	const int interval = 50;

//	LARGE_INTEGER start, end, freq;
//	::QueryPerformanceFrequency(&freq);
//	::QueryPerformanceCounter(&start);

	return ChaoticLib::Win32::MessageLoop(/*[&]{
		::QueryPerformanceCounter(&end);
		if((double)(end.QuadPart - start.QuadPart) * 1000 / freq.QuadPart > interval){
			start = end;
			::RedrawWindow(w.GetHwnd(), nullptr, nullptr, RDW_INVALIDATE | RDW_ALLCHILDREN);
		}else
			Sleep(1);
	}*/);
}

int APIENTRY wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int)
{
	if(FAILED(::CoInitialize(NULL)))
		return 0;
	if(!ChaoticLib::Initialize()){
		::CoUninitialize();
		return 0;
	}

	::AllocConsole();

	FILE *fp;
	::_wfreopen_s(&fp, L"CONOUT$", L"w", stdout);

	int r = Run();

	::fclose(fp);

	ChaoticLib::Uninitialize();
	::CoUninitialize();

	return r;
}
