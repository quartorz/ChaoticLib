#include "ChaoticLib\ChaoticLib.h"
#include "Sample.h"

#include <cstdio>

#if defined _M_IX86
# pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
# pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
# pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
# pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

int Run()
{
	MainWindow::Register();

	RECT rc = {0, 0, 800, 600};
	::AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW & ~WS_SIZEBOX, FALSE);

	MainWindow w;
	w.Create(0, WS_OVERLAPPEDWINDOW & ~(WS_SIZEBOX | WS_MAXIMIZEBOX), L"Sample", CW_USEDEFAULT, 0, rc.right - rc.left, rc.bottom - rc.top);
	w.Show();

	return ChaoticLib::Win32::MessageLoop(ChaoticLib::Win32::FrameController<100, 60>(w));
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
