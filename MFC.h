#pragma once
#include <iostream>
class CLObject
{
public: 
	CLObject::CLObject() { std::cout<< "CObject Constructor \n";}
	CLObject::~CLObject() { std::cout<< "CObject Destructor \n";}
};

class CLCmdTarget : public CLObject
{
public: 
	CLCmdTarget::CLCmdTarget() { std::cout<< "CLCmdTarget Constructor \n";}
	CLCmdTarget::~CLCmdTarget() { std::cout<< "CLCmdTarget Destructor \n";}
};

class CLWinThread : public CLCmdTarget
{
public:
	CLWinThread::CLWinThread() { std::cout<< "CLWinThread Constructor \n";}
	CLWinThread::~CLWinThread() { std::cout<< "CLWinThread Destructor \n";}
};

class CLWindApp : public CLWinThread
{
public:
	CLWindApp::CLWindApp() {  m_pCurrentWinApp=this; std::cout<< "CLWindApp Constructor \n";}
	CLWindApp::~CLWindApp() { std::cout<< "CLWindApp Destructor \n"; }
public:
	CLWindApp*m_pCurrentWinApp;
};

class CLDocument : public CLCmdTarget
{
public:
	CLDocument::CLDocument() { std::cout<< "CLDocument Constructor \n";}
	CLDocument::~CLDocument() { std::cout<< "CLDocument Destructor \n"; }
};

class CLWnd : public CLCmdTarget
{
public:
	CLWnd::CLWnd() { std::cout<< "CLWnd Constructor \n";}
	CLWnd::~CLWnd() { std::cout<< "CLWnd Destructor \n"; }
};

class CLFrameWnd : public CLCmdTarget
{
public:
	CLFrameWnd::CLFrameWnd() { std::cout<< "CLFrameWnd Constructor \n";}
	CLFrameWnd::~CLFrameWnd() { std::cout<< "CLFrameWnd Destructor \n"; }
};

class CLView : public CLWnd
{
public:
	CLView::CLView() { std::cout<< "CLView Constructor \n";}
	CLView::~CLView() { std::cout<< "CLView Destructor \n"; }
};

CLWindApp*AdfxGetApp();