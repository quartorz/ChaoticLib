#include "ChaoticLib\ChaoticLib.h"
#include "Window2.h"

int Run()
{
	Window::Register();

	Window w;
	w.Create(0);
	w.Show();

	return Win32::MessageLoop();
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
