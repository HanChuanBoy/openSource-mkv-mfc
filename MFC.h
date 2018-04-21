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
	virtual bool  InitInstance(){
		std::cout<< "CLWinThread InitInstance \n";
		return true;
	}
	virtual bool Run(){
		std::cout<< "CLWinThread Run \n";
		return true;
	}
};
class CLWnd;

class CLWindApp : public CLWinThread
{
public:
	CLWindApp::CLWindApp() {  m_pCurrentWinApp=this; std::cout<< "CLWindApp Constructor \n";}
	CLWindApp::~CLWindApp() { std::cout<< "CLWindApp Destructor \n"; }
public:
	CLWindApp*m_pCurrentWinApp;
	CLWnd*m_pMainWnd;
public:
	virtual bool InitInstance(){
		std::cout<< "CLWindApp InitInstance \n";
		return true;
	}
	virtual bool InitApplication(){
		std::cout<< "CLWindApp InitApplication \n";
		return true;
	}
	virtual bool Run(){
		std::cout<< "CLWindApp Run \n";
		return CLWinThread::Run();
	}
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
	virtual bool Create();
	bool CreateEx();
	virtual bool PreCreateWindow();
};

class CLFrameWnd : public CLWnd
{
public:
	CLFrameWnd::CLFrameWnd() { std::cout<< "CLFrameWnd Constructor \n";}
	CLFrameWnd::~CLFrameWnd() { std::cout<< "CLFrameWnd Destructor \n"; }
	bool CreateEx();
	virtual bool PreCreateWindow();
};

class CLView : public CLWnd
{
public:
	CLView::CLView() { std::cout<< "CLView Constructor \n";}
	CLView::~CLView() { std::cout<< "CLView Destructor \n"; }
};

CLWindApp*AdfxGetApp();