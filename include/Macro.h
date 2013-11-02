#pragma once

#include <type_traits>
#include <exception>

#if _MSC_VER <= 1600

#define final sealed

#endif

#ifdef _MSC_VER
template <class T>
inline void SafeRelease(T *&p)
{
	if(p != nullptr){
		p->Release();
		p = nullptr;
	}
}

// http://msdn.microsoft.com/ja-jp/library/windows/desktop/dd941735%28v=vs.85%29.aspx

// Acquires an additional reference, if non-null.
template <typename InterfaceType>
inline InterfaceType* SafeAcquire(InterfaceType* newObject)
{
    if (newObject != nullptr)
        newObject->AddRef();

    return newObject;
}


// Sets a new COM object, releasing the old one.
template <typename InterfaceType>
inline void SafeSet(InterfaceType** currentObject, InterfaceType* newObject)
{
    SafeAcquire(newObject);
    SafeRelease(&currentObject);
    currentObject = newObject;
}


// Maps exceptions to equivalent HRESULTs,
inline HRESULT ExceptionToHResult() throw()
{
    try
    {
        throw;  // Rethrow previous exception.
    }
    catch(std::bad_alloc&)
    {
        return E_OUTOFMEMORY;
    }
    catch (...)
    {
        return E_FAIL;
    }
}

#endif

template <class T>
inline typename std::make_signed<T>::type ToSigned(const T &v)
{
	return static_cast<typename std::make_signed<T>::type>(v);
}
