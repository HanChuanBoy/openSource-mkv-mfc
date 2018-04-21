#include "my.h"
using namespace std;

CMyWinApp theApp;
bool CMyWinApp::InitInstance(){
	std::cout<< "CMyWinApp InitInstance \n";
	m_pMainWnd=new CMyFrameWnd();
	return true;
}
void PrintAllClass(){
	CLRuntimeClass*pClass;
	for(pClass=CLRuntimeClass::pFirstClass;pClass!=NULL;pClass=pClass->m_pNextClass) {
		cout<< pClass->m_lpszClassName << "\n";
		cout<< pClass->m_nObjectSize << "\n";
		cout<< pClass->m_wSchema << "\n" ;
	}
}
void main()
{
	CLWindApp*pApp=AdfxGetApp();
	pApp->InitApplication();
	pApp->InitInstance();
	pApp->Run();
	PrintAllClass();
}