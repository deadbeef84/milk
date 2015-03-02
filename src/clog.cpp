#include "milk/obsolete/milk_clog.h"
#include "milk/helper.h"
#include "milk/boost.h"
#include <ctime>
using namespace milk;
using namespace std;

// CLog class implementation
///////////////////////////////////////////////

CLog::CLog(const char *filename, bool append, Level report)
: m_reportLevel(LNOTICE), m_reportMask(report), m_flushed(true)
{
	if (append && milk::fileExists(filename))
	{
		m_ofstream.open(filename, ios::app);
		m_ofstream << "\n\n";
	}
	else
		m_ofstream.open(filename, ios::out);

	char buf[128];
	time_t theTime = time(0);
	strftime(buf, 128, "%B %d %Y, %X", localtime(&theTime));

	m_ofstream << "-----------------------------"	<< '\n';
	m_ofstream << "  New log-session started"		<< '\n';
	m_ofstream << "  " << buf						<< '\n';
	m_ofstream << "-----------------------------"	<< '\n';
	m_ofstream << std::flush;
}

CLog::~CLog()
{
	flush();
}

void CLog::begin(const std::string& name)
{
	if (m_reportLevel & m_reportMask)
	{
		autoFlush();
		doIndent();
		m_ofstream<<name<<'\n';
		doIndent();
		m_ofstream<<"{\n";
		m_indent.push_back(true);
		flush();
	}
	else
		m_indent.push_back(false);
}

void CLog::end()
{
	BOOST_ASSERT(m_indent.size()!=0);
	if (m_indent.back())
	{
		autoFlush();
		m_indent.pop_back();
		doIndent();
		m_ofstream<<"}\n";
		flush();
	}
	else
		m_indent.pop_back();
}

void CLog::doIndent()
{
	for (list<bool>::iterator it=m_indent.begin() ; it!=m_indent.end() ; ++it)
		if (*it)
			m_ofstream<<"    ";
}

void CLog::flush()
{
	m_ofstream.flush();
	m_flushed = true;
}

void CLog::setReportLevel(Level report)
{ m_reportLevel = report; }
Level CLog::getReportLevel() const
{ return m_reportLevel; }

void CLog::setReportMask(Level report)
{ m_reportMask = report; }
Level CLog::getReportMask() const
{ return m_reportMask; }

void CLog::autoFlush()
{
	if(!m_flushed)
		flush();
}
