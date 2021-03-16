#include "MyBaseShader.h"

MyBaseShader::MyBaseShader(ID3D11Device* device, HWND hwnd) 
	:
	BaseShader::BaseShader(device, hwnd)
{	
}

MyBaseShader::~MyBaseShader()	
{	
	BaseShader::~BaseShader();
}