#pragma once

#include <Windows.h>
#include <WinInet.h>

#include <cassert>
#include <functional>
#include <chrono>
#include <future>

#pragma comment(lib,"WinInet")

namespace ChaoticLib{ namespace Win32{

	class Internet{
		using handler_type = std::function<bool(void *buffer, std::size_t length)>;
		handler_type get_handler = [](...){return false;};
		DWORD timeout = 30000;

		HINTERNET Open(const wchar_t *user_agent=L"ChaoticLib::Win32::Internet")
		{
			return ::InternetOpenW(
				(user_agent != nullptr ? user_agent : L"ChaoticLib::Win32::Internet"),
				INTERNET_OPEN_TYPE_PRECONFIG,
				nullptr,
				nullptr,
				0);
		}
		void Close(HINTERNET hInternet)
		{
			if(hInternet != nullptr)
				::InternetCloseHandle(hInternet);
		}

	public:
		Internet()
		{
		}
		~Internet()
		{
		}
		void SetGETHandler(const handler_type &h)
		{
			get_handler = h;
		}
		// time < 0でタイムアウトしなくなる
		void SetTimeout(std::chrono::milliseconds time)
		{
			auto c = time.count();
			timeout = (c >= 0? static_cast<DWORD>(c): 0xFFFFFFFF);
		}
		bool GET(const wchar_t *url, const wchar_t *user_agent=nullptr, bool ssl=false)
		{
			HINTERNET hInternet = Open(nullptr);

			if(hInternet == nullptr)
				return false;

			::InternetSetOptionW(
				hInternet,
				INTERNET_OPTION_CONNECT_TIMEOUT,
				&timeout,
				sizeof(timeout));
			::InternetSetOptionW(
				hInternet,
				INTERNET_OPTION_DATA_RECEIVE_TIMEOUT,
				&timeout,
				sizeof(timeout));

			HINTERNET hFile = ::InternetOpenUrlW(
				hInternet,
				url,
				nullptr,
				0,
				INTERNET_FLAG_RELOAD | INTERNET_FLAG_RAW_DATA | ((ssl) ? INTERNET_FLAG_SECURE : 0),
				0);
			if(hFile == nullptr){
				::InternetCloseHandle(hInternet);
				return false;
			}

			BOOL res;
			unsigned char buf[1002];
			DWORD readsize;
			for(;;){
				res = ::InternetReadFile(
					hFile,
					buf,
					1000,
					&readsize);
				if(!res || readsize == 0)
					break;

				buf[readsize] = '\0';
				buf[readsize + 1] = '\0';
				if(!get_handler(buf, static_cast<std::size_t>(readsize)))
					break;
			}

			::InternetCloseHandle(hFile);
			::InternetCloseHandle(hInternet);
			return true;
		}
		// 未完成
		void GETAsync(const wchar_t *url, const wchar_t *user_agent=nullptr, bool ssl=false)
		{
			std::thread(&Internet::GET, this, url, user_agent, ssl).detach();
		}
	};

} }
