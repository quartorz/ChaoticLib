#include "Win32\Window.h"
#include "Win32\Creator.h"
#include "Win32\MessageLoop.h"

using namespace ChaoticLib;

class Window:
	public Win32::Window<Window, Win32::Movable<Window>, Win32::QuitOnClose<Window>>,
	public Win32::Creator<Window>
{
public:
	static const wchar_t *classname;

	Window()
	{
	}

	void OnMove(int x, int y)
	{
		::OutputDebugStringW(L"move\n");
	}
};

const wchar_t *Window::classname = L"Window";

int APIENTRY wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int)
{
	Window::Register();

	Window w;
	w.Create(0);
	w.Show();

	Win32::MessageLoop();

	return 0;
}
