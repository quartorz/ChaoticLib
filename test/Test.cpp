#include "ChaoticLib\Win32\Window.h"
#include "ChaoticLib\Win32\Creator.h"
#include "ChaoticLib\Win32\MessageLoop.h"
#include "ChaoticLib\Direct2D\Painter.h"
#include "ChaoticLib\Direct2D\Traits.h"
#include "ChaoticLib\Direct2D\SolidBrush.h"
#include "ChaoticLib\ChaoticLib.h"
#include "ChaoticLib\ResourceManager.h"

using namespace ChaoticLib;

class Window:
	public Win32::Window<
		Window,
		Win32::Movable<Window>,
		Win32::QuitOnClose<Window>,
		Direct2D::Painter<Window>,
		ResourceManager<Window, Direct2D::Traits>
	>,
	public Win32::Creator<Window>
{
	Direct2D::SolidBrush *b;

public:
	static const wchar_t *classname;

	void Initialize()
	{
		b = CreateSolidBrush(Direct2D::Color(0, 0, 255));
	}

	void Uninitialize()
	{
		DeleteResource(b);
	}

	void OnMove(int x, int y)
	{
		::OutputDebugStringW(L"move\n");
	}

	void Draw(const PaintStruct &ps)
	{
		Clear(ps, Direct2D::Color(1.f));

		int w, h;
		std::tie(w, std::ignore) = GetSize();
		DrawLine(ps, *b, Direct2D::Point(0, 0), Direct2D::Point(w / 2, 100));
	}
};

const wchar_t *Window::classname = L"Window";

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
