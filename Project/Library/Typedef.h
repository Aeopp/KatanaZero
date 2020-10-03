#pragma once
#include <d3dx9.h>
#include <d3d9.h>
#include <ostream>
#include <fstream>
#include <istream>
#include <iostream>

using vec2=  D3DXVECTOR2 ;
using vec3= D3DXVECTOR3 ;
using vec4= D3DXVECTOR4 ;
using matrix =D3DXMATRIX ;

static std::wostream& operator<<(std::wostream& os,
	const vec3& _vec3)
{
	return os

		// ���� ���� ����
		<< _vec3.x << std::endl
		<< _vec3.y << std::endl
		<< _vec3.z << std::endl; 
	// ���� ���� ��
};

static std::wistream& operator >> (std::wistream& is,
	vec3& _vec3)
{
	return is
		>> _vec3.x
		>> _vec3.y
		>> _vec3.z;
};




