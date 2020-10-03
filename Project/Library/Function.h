#pragma once
#include <type_traits>
using uint32_t = unsigned int;

// Release 기능을 수행하는 베이스 클래스를 찾게된다면 타입 검사하는 코드를 추가할것
template<typename DX_Release_EnableType>
uint32_t DXRelease(DX_Release_EnableType& Instance)
{
	// static_assert (std::is_base_of<DirectXReleaseBaseClass , DX_Release_EnableType >::type, L"");

	uint32_t RefCnt = 0; 

	if (Instance)
	{
		RefCnt = Instance->Release();
		Instance = nullptr;
	}

	return RefCnt;
}

template<typename T>
void SAFE_DELETE(T& Instance)
{
	static_assert(std::is_pointer<T>::value, __FUNCTION__);

	if (Instance)
	{
		delete Instance; 
		Instance = nullptr; 
	}
}