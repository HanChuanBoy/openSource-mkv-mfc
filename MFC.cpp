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
CLWindApp*AdfxGetApp(){
	return theApp.m_pCurrentWinApp;
}