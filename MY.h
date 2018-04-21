#pragma once
#include <iostream>
#include "mfc.h"

class CMyWinApp : public CLWindApp
{
public:
	CMyWinApp::CMyWinApp() { std::cout<< "CMyWinApp Constructor \n";}
	CMyWinApp::~CMyWinApp() { std::cout<< "CMyWinApp Destructor \n"; }
	virtual bool InitInstance();
};

class CMyFrameWnd : public CLFrameWnd
{
public:
	CMyFrameWnd::CMyFrameWnd() { std::cout<< "CMyFrameWnd Constructor \n";}
	CMyFrameWnd::~CMyFrameWnd() { std::cout<< "CMyFrameWnd Destructor \n"; }
};