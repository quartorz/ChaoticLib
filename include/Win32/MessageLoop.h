#pragma once

#include <Windows.h>

#include <chrono>
#include <thread>

namespace ChaoticLib{ namespace Win32{

	template <class... Func>
	// アイドル時にfnを実行
	inline int MessageLoop(Func... fn)
	{
		auto funcs = ChaoticLib::TMP::MakeCaller(fn...);

		BOOL ret;
		MSG msg;
		do{
			if(::PeekMessageW(&msg, nullptr, 0, 0, PM_NOREMOVE)){
				ret = ::GetMessageW(&msg, nullptr, 0, 0);
				if(ret == 0 || ret == -1)
					break;
				::TranslateMessage(&msg);
				::DispatchMessageW(&msg);
			}else{
				funcs();
			}
		}while(msg.message != WM_QUIT);

		return msg.wParam;
	}

	inline int MessageLoop(void)
	{
		BOOL ret;
		MSG msg;
		while((ret = ::GetMessageW(&msg, nullptr, 0, 0)) == 1){
			::TranslateMessage(&msg);
			::DispatchMessageW(&msg);
		}

		return msg.wParam;
	}

	template <class Window, unsigned ID = UINT_MAX /* std::numeric_limits<unsigned>::max() */, unsigned FPS = 60>
	class FrameControllerClass{
		Window &window;
		std::chrono::system_clock::time_point first;
		int count = 0;
		bool flag = true;

	public:
		FrameControllerClass(Window &w): window(w)
		{
		}

		void operator()()
		{
			using namespace std::chrono;
			using std::this_thread::sleep_for;

			milliseconds duration;

			if(count == 0){
				if(flag){
					flag = false;
				}else{
					duration = duration_cast<milliseconds>(first - system_clock::now()) + seconds(1);
				}
			}else{
				duration = duration_cast<milliseconds>(first - system_clock::now()) + count * milliseconds(1000) / FPS;
			}

			sleep_for(duration);

			auto now = system_clock::now();

			if(count == 0)
				first = now;

			++count;
			if(count >= FPS)
				count = 0;

			window.WindowProc(window.GetHwnd(), WM_TIMER, ID, 0);
			window.Repaint();
		}
	};

	template <unsigned ID, unsigned FPS, class Window>
	FrameControllerClass<Window, ID, FPS> FrameController(Window &window)
	{
		return FrameControllerClass<Window, ID, FPS>(window);
	}

} }
