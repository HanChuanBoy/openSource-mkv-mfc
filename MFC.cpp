#include "my.h"
using namespace std;
/*
  <>引用的是编译器的类库路径里面的头文件;
  ""引用的是你程序目录的相对路径的头文件;
  如果是用""先在程序目录中找,找不到区编译器类库路径找;
*/
bool CLWnd::Create(){
	std::cout<< "CLWnd Create \n";
	return true;
}
bool CLWnd::CreateEx(){
	std::cout<< "CLWnd CreateEx \n";
	PreCreateWindow();
	return true;
}
bool CLWnd::PreCreateWindow(){
	std::cout<< "CLWnd PreCreateWindow \n";
	return true;
}

bool CLFrameWnd::CreateEx(){
	std::cout<< "CLFrameWnd PreCreateWindow \n";
	return true;
}

bool CLFrameWnd::PreCreateWindow(){
	std::cout<< "CLFrameWnd PreCreateWindow \n";
	return true;
}

extern CMyWinApp theApp;
static char szCObject[]="CLObject";
struct CLRuntimeClass CLObject::classCLObject=
{szCObject,sizeof(CLObject),0xffff,NULL,NULL,NULL};
static ALFX_CLASSINIT _init_CObject(&CLObject::classCLObject);
CLRuntimeClass*CLRuntimeClass::pFirstClass=NULL;
ALFX_CLASSINIT::ALFX_CLASSINIT(CLRuntimeClass*pNewClass){
	pNewClass->m_pNextClass=CLRuntimeClass::pFirstClass;
	CLRuntimeClass::pFirstClass=pNewClass;
}
CLRuntimeClass*CLObject::GetRuntimeClass() const {
	return &CLObject::classCLObject;
}

ILMPLEMENT_DYNAMIC(CLCmdTarget,CLObject)     //一方面声明了很多静态的名称;
ILMPLEMENT_DYNAMIC(CLWinThread,CLCmdTarget)
ILMPLEMENT_DYNAMIC(CLWindApp,CLWinThread)
ILMPLEMENT_DYNAMIC(CLWnd,CLCmdTarget)
ILMPLEMENT_DYNAMIC(CLFrameWnd,CLWnd)
ILMPLEMENT_DYNAMIC(CLDocument,CLCmdTarget)
ILMPLEMENT_DYNAMIC(CLView,CLWnd)

CLWindApp*AdfxGetApp(){
	return theApp.m_pCurrentWinApp;
}