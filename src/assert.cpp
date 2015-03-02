#include "milk/includes.h"
#include "milk/error.h"
#include "milk/boost.h"

namespace boost
{
	void assertion_failed(char const* expr, char const* function, char const* file, long line)
	{
		milk::ExceptionReporter::stackDump();

		std::string msg = std::string(expr)+"\nFunction "+function+" failed.\n";
		msg += "Line " + boost::lexical_cast<std::string>(line) + " in file " + file + ".";

		std::string title = "Debug assertion failed";

		milk::CLog* log = milk::error::milk::getLog();
		if (log)
			*log << milk::LERROR << title << ":\n" << msg << std::endl;

#ifdef WIN32
		MessageBox(NULL, msg.c_str(), title.c_str(), MB_OK|MB_ICONINFORMATION);
#else
		printf((title+":\n"+msg).c_str());
#endif

		abort();
	}
}
