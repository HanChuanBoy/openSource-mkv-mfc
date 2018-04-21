#pragma once
#include <iostream>
#define BOOL int
#define TRUE 1
#define FALSE 0
#define LPCSTR LPSTR
typedef char *LPSTR;
#define UINT int
#define PASCAL _stdcall
class CLObject;
struct CLRuntimeClass
{
	LPCSTR m_lpszClassName;
	int m_nObjectSize;
	UINT m_wSchema;
	CLObject*(PASCAL*m_pfnCreateObject)();
	CLRuntimeClass*m_pBaseClass;
	static CLRuntimeClass*pFirstClass;
	CLRuntimeClass*m_pNextClass;
};
struct ALFX_CLASSINIT
{ ALFX_CLASSINIT(CLRuntimeClass*pNewClass);};

#define RLUNTIME_CLASS(class_name) \
   (&class_name::class##class_name)
#define DECLEAR_DLYNAMIC(class_name) \
   public:\
   static CLRuntimeClass class##class_name; \
   virtual CLRuntimeClass*GetRuntimeClass() const;
#define _ILMPLEMENT_RUNTIMECLASS(class_name,base_class_name,wSchema,pfnNew) \
	static char _lpsz##class_name[]=#class_name; \
	CLRuntimeClass class_name::class##class_name = { \
	_lpsz##class_name, sizeof(class_name),wSchema,pfnNew,RLUNTIME_CLASS(base_class_name),\
	 NULL }; \
	 static ALFX_CLASSINIT _init##class_name(&class_name::class##class_name);\
	 CLRuntimeClass*class_name::GetRuntimeClass() const \
	 {return &class_name::class##class_name;} \
#define ILMPLEMENT_DYNAMIC(class_name,base_class_name) \
    _ILMPLEMENT_RUNTIMECLASS(class_name,base_class_name,0xFFFF,NULL)
class CLObject
{
public: 
	CLObject::CLObject() { std::cout<< "CObject Constructor \n";}
	CLObject::~CLObject() { std::cout<< "CObject Destructor \n";}
	virtual CLRuntimeClass*GetRuntimeClass() const;
public:
	static CLRuntimeClass classCLObject;
};
class CLCmdTarget : public CLObject
{
	DECLEAR_DLYNAMIC(CLCmdTarget)
public: 
	CLCmdTarget::CLCmdTarget() { std::cout<< "CLCmdTarget Constructor \n";}
	CLCmdTarget::~CLCmdTarget() { std::cout<< "CLCmdTarget Destructor \n";}
};
class CLWinThread : public CLCmdTarget
{
	DECLEAR_DLYNAMIC(CLWinThread)
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
	DECLEAR_DLYNAMIC(CLWindApp)
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
	DECLEAR_DLYNAMIC(CLDocument)
public:
	CLDocument::CLDocument() { std::cout<< "CLDocument Constructor \n";}
	CLDocument::~CLDocument() { std::cout<< "CLDocument Destructor \n"; }
};

class CLWnd : public CLCmdTarget
{
	DECLEAR_DLYNAMIC(CLWnd)
public:
	CLWnd::CLWnd() { std::cout<< "CLWnd Constructor \n";}
	CLWnd::~CLWnd() { std::cout<< "CLWnd Destructor \n"; }
	virtual bool Create();
	bool CreateEx();
	virtual bool PreCreateWindow();
};

class CLFrameWnd : public CLWnd
{
	DECLEAR_DLYNAMIC(CLFrameWnd)
public:
	CLFrameWnd::CLFrameWnd() { std::cout<< "CLFrameWnd Constructor \n";}
	CLFrameWnd::~CLFrameWnd() { std::cout<< "CLFrameWnd Destructor \n"; }
	bool CreateEx();
	virtual bool PreCreateWindow();
};

class CLView : public CLWnd
{
	DECLEAR_DLYNAMIC(CLView)
public:
	CLView::CLView() { std::cout<< "CLView Constructor \n";}
	CLView::~CLView() { std::cout<< "CLView Destructor \n"; }
};

CLWindApp*AdfxGetApp();