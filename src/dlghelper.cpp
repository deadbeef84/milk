#include "milk/dlghelper.h"
using namespace milk;
using namespace std;

#ifdef WIN32
#	include <windows.h>
	void dialog::alert(const string& title, const string& msg)
	{
		MessageBox(NULL, msg.c_str(), title.c_str(), MB_OK|MB_ICONINFORMATION);
	}
	bool dialog::confirm(const string& title, const string& msg)
	{
		return (MessageBox(NULL, msg.c_str(), title.c_str(), MB_OKCANCEL|MB_ICONINFORMATION) == IDOK);
	}
#else
#	include <cstdio>
	void dialog::alert(const string& title, const string& msg)
	{
		printf("[%s] %s\n", title.c_str(), msg.c_str());
	}
	bool dialog::confirm(const string& title, const string& msg)
	{
		char inp = 0;
		printf("[%s] %s\n", title.c_str(), msg.c_str());
		while(!inp)
		{
			printf("\r(O)k, (C)ancel: ");
			inp = getchar();
			inp = (inp == 'O' || inp == 'o') ? 1 : (inp == 'C' || inp == 'c') ? 2 : 0;
		}
		return inp==1 ? true : false;
	}
#endif
