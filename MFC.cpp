#include "my.h"
using namespace std;
/*
  <>引用的是编译器的类库路径里面的头文件;
  ""引用的是你程序目录的相对路径的头文件;
  如果是用""先在程序目录中找,找不到区编译器类库路径找;
*/
extern CMyWinApp theApp;
CLWindApp*AdfxGetApp(){
	return theApp.m_pCurrentWinApp;
}