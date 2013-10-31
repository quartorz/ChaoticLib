#pragma once

#include <Windows.h>

namespace ChaoticLib{ namespace Win32{

	template <typename Func>
	// アイドル時にfnを実行
	inline int MessageLoop(Func fn)
	{
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
				fn();
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

} }
