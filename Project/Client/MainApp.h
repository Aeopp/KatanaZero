#pragma once
#include "singleton_interface.h"

class App :public  singleton_interface<App >
{
public :
	HRESULT Initialize();
	void Update(); 
	void LateUpdate(); 
	void Render(); 
	void Release(); 
};

