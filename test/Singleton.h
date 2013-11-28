#pragma once

#include <memory>

#if !defined SELECTANY
#if defined _MSC_VER
#define SELECTANY __declspec(selectany)
#elif defined __GNUC__
#define SELECTANY __attribute__((selectany))
#endif
#endif

template <class Derived>
class Singleton{
	static std::unique_ptr<Derived> instance;

public:
	static Derived *GetInstance()
	{
		if(!instance){
			instance = std::unique_ptr<Derived>(new Derived);
		}
		return instance.get();
	}
	static void DeleteInstance()
	{
		instance.release();
	}
};

template <class Derived>
SELECTANY std::unique_ptr<Derived> Singleton<Derived>::instance;
