#include "my.h"
using namespace std;

CMyWinApp theApp;
bool CMyWinApp::InitInstance(){
	std::cout<< "CMyWinApp InitInstance \n";
	m_pMainWnd=new CMyFrameWnd();
	return true;
}
void main()
{
	CLWindApp*pApp=AdfxGetApp();
	pApp->InitApplication();
	pApp->InitInstance();
	pApp->Run();
}