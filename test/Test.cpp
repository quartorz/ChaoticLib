#include "ChaoticLib\ChaoticLib.h"
#include "Sample.h"

#include <cstdio>

int Run()
{
	MainWindow::Register();

	RECT rc = {0, 0, 800, 600};
	::AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW & ~WS_SIZEBOX, FALSE);

	MainWindow w;
	w.Create(0, WS_OVERLAPPEDWINDOW & ~(WS_SIZEBOX | WS_MAXIMIZEBOX), L"Sample", CW_USEDEFAULT, 0, rc.right - rc.left, rc.bottom - rc.top);
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

GUID DXGI_DEBUG_ALL_X = {0xe48ae283, 0xda80, 0x490b, {0x87, 0xe6, 0x43, 0xe9, 0xa9, 0xcf, 0xda, 0x8}};

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

	{/*
		ChaoticLib::Direct2D::
			fac;
		while(fac[D2D]->Release() > 2);*/
	}

	::fclose(fp);

	ChaoticLib::Uninitialize();
	::CoUninitialize();

	return r;
}
