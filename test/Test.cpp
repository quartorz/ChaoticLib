#include "ChaoticLib\ChaoticLib.h"
#include "Window2.h"

int Run()
{
	Window::Register();

	Window w;
	w.Create(0);
	w.Show();

//	const int interval = 50;

//	LARGE_INTEGER start, end, freq;
//	::QueryPerformanceFrequency(&freq);
//	::QueryPerformanceCounter(&start);

	return Win32::MessageLoop(/*[&]{
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
	if(!Initialize()){
		::CoUninitialize();
		return 0;
	}

	int r = Run();

	Uninitialize();
	::CoUninitialize();

	return r;
}
