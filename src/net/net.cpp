#include "milk/net/net.h"
#include "milk/net/socket.h"
#include "milk/error.h"
#include "milk/helper.h"
#include <string>
using namespace milk;
using namespace std;

void Net::init()
{
#ifdef WIN32
    WSAData wsaData; // then try this instead

	//use version 1.1, compatible with BSD-sockets...
    if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0)
		throw error::net("Could not initialise WinSock");
	if (wsaData.wVersion != MAKEWORD(1, 1))
		throw error::net("Could not initialise WinSock 1.1");
#endif
}

void Net::free()
{
#ifdef WIN32
	WSACleanup();
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

CIP::CIP(int port)
{
	server(port);
}

CIP::CIP(const string& hostname, int port)
{
	resolve(hostname, port);
}

void CIP::server(int port)
{
	m_ip.sin_family = AF_INET;
	m_ip.sin_port = htons(static_cast<ushort>(port));
	m_ip.sin_addr.s_addr = INADDR_ANY;;
	memset(&(m_ip.sin_zero), '\0', 8);
}

void CIP::resolve(const string& name, int port)
{
	in_addr address;
	address.s_addr = inet_addr(name.c_str());
		
	hostent *hostEnt=0;
	
	//if not an ip-adress:
	if (address.s_addr == INADDR_NONE)
		hostEnt = gethostbyname(name.c_str());
	else
		hostEnt = gethostbyaddr(reinterpret_cast<const char *>(&address), sizeof(in_addr), AF_INET);
		
	if (hostEnt && hostEnt->h_addr_list[0])
	{
		m_ip.sin_family = AF_INET;
		m_ip.sin_port = htons(static_cast<ushort>(port));
		m_ip.sin_addr = *reinterpret_cast<in_addr *>(hostEnt->h_addr_list[0]);
		memset(&(m_ip.sin_zero), '\0', 8);
	}
	else
		throw error::net("Could not resolve "+name+":"+boost::lexical_cast<string>(port)+Net::errorStr());
}

Uint32 CIP::getIP() const
{		
	return ntohl(m_ip.sin_addr.s_addr);
}

Uint16 CIP::getPort() const
{
	return ntohs(m_ip.sin_port);
}

string CIP::getIPAsString() const
{		
	Uint32 ipaddr = getIP();
	return (boost::lexical_cast<string>(ipaddr>>24)+"."+boost::lexical_cast<string>((ipaddr>>16)&0xff)+"."+boost::lexical_cast<string>((ipaddr>>8)&0xff)+"."+boost::lexical_cast<string>(ipaddr&0xff));
}

string CIP::getIPAndPort() const
{
	return (getIPAsString()+":"+boost::lexical_cast<string>(getPort()));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

CSocketSet::CSocketSet()
{
	clear();
}

CSocketSet::~CSocketSet()
{
	//clear(); //should we clear?
}

void CSocketSet::check(int timeout)
{
	if(!m_set.empty())
	{
		FD_ZERO(&m_in);
		for(set<int>::iterator it = m_set.begin(); it != m_set.end(); ++it)
		{

#ifdef _MSC_VER
#pragma warning(disable: 4127)
#endif

			FD_SET(static_cast<uint>(*it), &m_in);

#ifdef _MSC_VER
#pragma warning(default: 4127)
#endif

		}

		timeval time;
		time.tv_sec		= timeout/1000;
		time.tv_usec	= (timeout%1000)*1000;

		//check in-streams
		int max_socket_value = *max_element(m_set.begin(), m_set.end());
		if (select(max_socket_value + 1, &m_in, 0, 0, &time) == -1)
			throw error::net("Call to select failed"+Net::errorStr());
	}
}

void CSocketSet::add(const ISocket& sock)
{
	m_set.insert(sock.m_socket);
}

void CSocketSet::remove(const ISocket& sock)
{
	m_set.erase(sock.m_socket);
}

void CSocketSet::clear()
{
	m_set.clear();
	FD_ZERO(&m_in);
}

bool CSocketSet::contains(const ISocket& sock)
{
	return (m_set.count(sock.m_socket)==1);
}

bool CSocketSet::isSet(const ISocket& sock)
{
	int isset = FD_ISSET(sock.m_socket, &m_in);
	return isset != 0;
}

/////////////////////////////////////////////////////

ICounted::ICounted() : m_outBytes(0), m_inBytes(0), m_inRate(0.0), m_outRate(0.0)
{

}

double ICounted::getInRate() const
{
	updateRate();
	return m_inRate;
}

double ICounted::getOutRate() const
{
	updateRate();
	return m_outRate;
}

void ICounted::addOut(size_t nbytes)
{
	m_outBytes += nbytes;
}

void ICounted::addIn(size_t nbytes)
{
	m_inBytes += nbytes;
}

void ICounted::updateRate() const
{
	double t = m_rateTimer.time();
	if(t > 1.0)
	{
		m_inRate = m_inBytes / t;
		m_outRate = m_outBytes / t;
		m_outBytes = m_inBytes = 0;
		m_rateTimer.reset();
	}
}
