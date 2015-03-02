#ifndef MILK_DLGHELPER_H_
#define MILK_DLGHELPER_H_

#include <string>
#include "milk/includes.h"

namespace milk
{
	namespace dialog
	{
		extern void alert(const std::string& title, const std::string& msg);
		extern bool confirm(const std::string& title, const std::string& msg);
	}
}

#endif
