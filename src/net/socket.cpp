#include "milk/includes.h"
#include "milk/net/socket.h"
#include "milk/net/net.h"
#include "milk/boost.h"
using namespace milk;
using namespace std;

#ifdef _MSC_VER
#pragma warning(disable: 4355)
#endif

CSocketTCP::CSocketTCP()
	: m_open(false), m_nagle(false), m_status(DISCONNECTED),
	m_connectThread(function_member(*this, &CSocketTCP::connect_private))
{
}

#ifdef _MSC_VER
#pragma warning(default: 4355)
#endif

CSocketTCP::~CSocketTCP()
{
	close();
}

void CSocketTCP::open(const CIP& ip)
{
	close();

	//TCP socket
	m_socket = static_cast<int>(socket(AF_INET, SOCK_STREAM, 0));
	if (m_socket == -1)
		throw error::net("Could not create TCP-socket"+Net::errorStr());

	if (bind(m_socket, reinterpret_cast<const sockaddr*>(ip.getAddress()), sizeof(sockaddr)) == -1)
	{
#ifdef WIN32
		if (WSAGetLastError() == WSAEADDRINUSE)
			throw error::net("Adress already in use. Please try another port");
#endif
		throw error::net("Could not bind socket to local ip"+Net::errorStr());
	}

	//queue 20 ppl at max
	if (listen(m_socket, 20) == -1)
		throw error::net("Could not set serversocket to listen"+Net::errorStr());

	m_open = true;
}

void CSocketTCP::connect(const CIP& ip)
{
	close();
	m_status = PENDING;
	m_remoteIP = ip;
	m_connectThread.start();
}

void CSocketTCP::connect_private()
{
	//TCP socket
	m_socket = static_cast<int>(socket(AF_INET, SOCK_STREAM, 0));
	if (m_socket != -1)
	{
		if (::connect(m_socket, reinterpret_cast<const sockaddr*>(m_remoteIP.getAddress()), sizeof(sockaddr)) != -1)
		{
			//success!
			setStatus(CONNECTED);
			setNagle(false);
			return;
		}
	}

	setStatus(DISCONNECTED);
}

void CSocketTCP::setStatus(ConnectionStatus newStatus)
{
	CMutexLock lock(m_connectMutex);
	m_status = newStatus;
}

ConnectionStatus CSocketTCP::status()
{
	ConnectionStatus theStatus = status_private();

	// finnished connecting
	if (m_connectThread.running() && theStatus != PENDING)
	{
		m_connectThread.wait();
	}

	return m_status;
}

ConnectionStatus CSocketTCP::status_private() const
{
	CMutexLock lock(m_connectMutex);
	return m_status;
}

bool CSocketTCP::open() const
{
	BOOST_ASSERT(status_private()==DISCONNECTED);
	return m_open;
}

void CSocketTCP::close()
{
	if (m_connectThread.running())
		m_connectThread.wait();

	ConnectionStatus s = status();

	BOOST_ASSERT(s != PENDING);

	if (s==CONNECTED || m_open)
	{
#ifdef WIN32
		shutdown(m_socket, SD_BOTH);
		if (closesocket(m_socket) != 0)
#else
		if (::close(m_socket) == -1);
#endif
			throw error::net("Could not close socket"+Net::errorStr());
	}

	m_open = false;
	m_status = DISCONNECTED;
}

void CSocketTCP::accept(CSocketTCP& client)
{
	BOOST_ASSERT(status_private()==DISCONNECTED);

#ifdef WIN32
	typedef int socklen_t;
#endif

	client.close();
	int size = sizeof(sockaddr_in);
	sockaddr_in addr;

	client.m_socket = static_cast<int>(::accept(m_socket, reinterpret_cast<sockaddr*>(&addr),
		reinterpret_cast<socklen_t*>(&size)));

	if (client.m_socket == -1)
		throw error::net("Could not accept new client"+Net::errorStr());

	client.m_status = CONNECTED;

	client.setNagle(false);
}

CIP CSocketTCP::getAddress() const
{
	BOOST_ASSERT(status_private()==CONNECTED);

#ifdef WIN32
	typedef int socklen_t;
#endif

	CIP ip;
	int size = sizeof(sockaddr);
	if (getpeername(m_socket, reinterpret_cast<sockaddr*>(ip.getAddress()),
		reinterpret_cast<socklen_t*>(&size)) == -1)
	{
		throw error::net("getpeername failed"+Net::errorStr());
	}

	return ip;
}

void CSocketTCP::send(const void *data, uint nbytes)
{
	BOOST_ASSERT(status_private()==CONNECTED);

	int parameter = 0;
#ifndef WIN32
	if (m_nagle)
		parameter = MSG_DONTWAIT;
#endif

	//*

	int sent = 0;	// how many bytes we have sent

	while (sent < static_cast<int>(nbytes))
	{

		BOOST_ASSERT(sent==0);

		int n = ::send(m_socket, reinterpret_cast<const char*>(data)+sent, static_cast<int>(nbytes)-sent, parameter);
		if (n == -1)
		{
#ifdef WIN32
			int error = WSAGetLastError();
			if (error==WSAECONNRESET || error==WSAECONNABORTED || error==WSAENETRESET)
				throw error::net_disconnect("Disconnected");
#endif
			throw error::net_send("Could not send data"+Net::errorStr());
		}
		else if (n == 0)
			throw error::net_send("Send returned 0!?"+Net::errorStr());

		BOOST_ASSERT(n>0);

		sent += n;
	}

	/*/

	int n = ::send(m_socket, reinterpret_cast<const char*>(data), nbytes, parameter);
	if (n == -1)
	{
#ifdef WIN32
		if (WSAGetLastError()==WSAECONNRESET)
			throw error::net_disconnect("Disconnected");
#endif
		throw error::net_send("Could not send data"+Net::errorStr());
	}
	else if (n == 0)
		throw error::net_send("Send returned 0!?"+Net::errorStr());

	/**/
}

void CSocketTCP::send(const CPacket& packet)
{
	BOOST_ASSERT(status_private()==CONNECTED);

	CPacket::SizeHeader_t size = static_cast<CPacket::SizeHeader_t>(packet.size());

	int headerSize = sizeof(CPacket::SizeHeader_t);

	//*

	packet.putSizeFront();
	send(packet.getDataPtr()-headerSize, size+headerSize);

	/*/

	send(reinterpret_cast<const void*>(&size), sizeof(size));
	send(reinterpret_cast<const void*>(packet.getDataPtr()), size);

	/**/
}


uint CSocketTCP::recv(void *data, uint maxlen)
{
	BOOST_ASSERT(status_private()==CONNECTED);

	if (maxlen==0)
		return 0;

	//*

	recv_private(maxlen);

	BOOST_ASSERT(!m_buffer.empty());

	uint howMuch = std::min(maxlen, static_cast<uint>(m_buffer.size()));

	memcpy(data, &m_buffer[0], howMuch);
	m_buffer.erase(m_buffer.begin(), m_buffer.begin()+howMuch);

	return howMuch;

	/*/

	int nr = ::recv(m_socket, reinterpret_cast<char*>(data), maxlen, 0);
	if (nr == 0)
	{
		throw error::net_disconnect("Disconnected");
	}
	else if (nr == -1)
	{
		#ifdef WIN32
		if (WSAGetLastError()==WSAECONNRESET)
		throw error::net_disconnect("Disconnected");
		#endif
		throw error::net_recv("Could not receive data"+Net::errorStr());
	}
	return nr;

	/**/
}

bool CSocketTCP::recv(CPacket& packet)
{
	BOOST_ASSERT(status_private()==CONNECTED);

	packet.setReadPos(0);
	packet.resize(0);

	/*

	if (false)// sizeof(Uint32) <= m_buffer.size())
	{
		Uint32 packetSize;
		memcpy(&packetSize, &m_buffer[0], sizeof(Uint32));
		recv_private(2*packetSize+1024);
	}
	else
	{
		//512 bytes should be a big enough buffer for most packages
		recv_private(512);
	}

	BOOST_ASSERT(!m_buffer.empty());


	if (sizeof(Uint32) <= m_buffer.size())
	{
		Uint32 packetSize;
		memcpy(&packetSize, &m_buffer[0], sizeof(Uint32));

		if (packetSize == 0)
		{
			return true;
		}

		if (sizeof(Uint32) + packetSize  <= m_buffer.size())
		{
			packet.resize(packetSize);
			memcpy(packet.getDataPtr(), &m_buffer[0] + sizeof(Uint32), packetSize);
			m_buffer.erase(m_buffer.begin(), m_buffer.begin() + packetSize + sizeof(Uint32));
			return true;
		}
		BOOST_ASSERT(m_buffer.empty());
		return false;
	}
	BOOST_ASSERT(m_buffer.empty());
	return false;

	/*/

	/* the code above is flawed, i think the code
	below is flawless, and is the best we can do */

	if (sizeof(CPacket::SizeHeader_t) > m_buffer.size())
	{
		recv_private(static_cast<uint>(sizeof(CPacket::SizeHeader_t)-m_buffer.size()));
	}

	if (sizeof(CPacket::SizeHeader_t) <= m_buffer.size())
	{
		CPacket::SizeHeader_t size;
		memcpy(&size, &m_buffer[0], sizeof(size));

		if (size==0)
		{
			return true;
		}

		if (sizeof(size) + size > m_buffer.size())
		{
			// this might have to wait for some more data to recevie.
			// can we make it so that recv_private is only called once?

			recv_private(static_cast<uint>(sizeof(size) + size - m_buffer.size()));
		}

		if (sizeof(size) + size <= m_buffer.size())
		{
			packet.resize(size);
			memcpy(packet.getDataPtr(), &m_buffer[0] + sizeof(size), size);
			m_buffer.erase(m_buffer.begin(), m_buffer.begin() + sizeof(size) + size);

			return true;
		}
		return false;
	}
	return false;

	/**/

	/*
	if (sizeof(Uint32) > m_buffer.size())
	{
		recv_private(sizeof(Uint32));
	}

	if (sizeof(Uint32) <= m_buffer.size())
	{
		Uint32 size;
		memcpy(&size, &m_buffer[0], sizeof(size));

		if (size==0)
		{
			return true;
		}

		if (sizeof(Uint32) + size > m_buffer.size())
		{
			// this might have to wait for some more data to recevie.
			// can we make it so that recv_private is only called once?

			recv_private(size);
		}

		if (sizeof(Uint32) + size <= m_buffer.size())
		{
			packet.resize(size);
			memcpy(packet.getDataPtr(), &m_buffer[0] + sizeof(size), size);
			m_buffer.erase(m_buffer.begin(), m_buffer.begin() + sizeof(size) + size);

			return true;
		}
		return false;
	}
	return false;
	*/

	/*

	Uint32 size;
	if (recv(&size, sizeof(size)) != sizeof(size))
	{
		throw error::milk("To small packet-size received");
	}
	packet.resize(size);
	if (recv(packet.getDataPtr(), size) != size)
	{
		throw error::milk("To small packet-data received");
	}
	return true;
	*/
}

void CSocketTCP::recv_private(uint maxlen)
{
	BOOST_ASSERT(status_private()==CONNECTED);

	if (maxlen==0)
		return;

	uint oldsize = static_cast<uint>(m_buffer.size());

	m_buffer.resize(oldsize+maxlen);

	int nr = ::recv(m_socket, reinterpret_cast<char*>(&m_buffer[0]+oldsize), maxlen, 0);

	if (nr == 0)
	{
		throw error::net_disconnect("Disconnected");
	}
	else if (nr == -1)
	{
#ifdef WIN32
		int error = WSAGetLastError();
		if (error==WSAECONNRESET || error==WSAECONNABORTED || error==WSAENETRESET)
			throw error::net_disconnect("Disconnected");
#endif
		throw error::net_recv("Could not receive data"+Net::errorStr());
	}

	m_buffer.resize(oldsize+nr);
}

void CSocketTCP::setNagle(bool nagle)
{
	BOOST_ASSERT(status_private()==CONNECTED);

	m_nagle = nagle;

#ifdef WIN32
	/* the next stuff sets the socket to send directly,
	instead of waiting for a filled up ip-packet. */
	//*
	int on = (nagle ? 1 : 0);
	if (setsockopt(m_socket, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char*>(&on), sizeof(int)) != 0)
		throw error::net("setsockopt failed"+Net::errorStr());
	/**/
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/*
CSocketUDP::CSocketUDP() : m_socket(0)
{ }

CSocketUDP::CSocketUDP(Uint16 port) : m_socket(0)
{ open(port); }

CSocketUDP::~CSocketUDP()
{ close(); }

void CSocketUDP::open(Uint16 port)
{
	close();
	m_socket = SDLNet_UDP_Open(port);
	if (!m_socket)
		throw error::net((string)"Could not open UDP socket, SDL_Net error: " + SDLNet_GetError());
}

bool CSocketUDP::open() const
{ return (m_socket!=0); }

void CSocketUDP::close()
{
	if (open())
	{
		SDLNet_UDP_Close(m_socket);
		m_socket=0;
	}
}

void CSocketUDP::bind(const CIP& ip)
{
	unbind();
	if (SDLNet_UDP_Bind(m_socket, 0, ip.getIPPointer()) != 0)
		throw error::net((string)"Could not bind UDP socket, SDL_Net error: " + SDLNet_GetError());
}

void CSocketUDP::unbind()
{
	SDLNet_UDP_Unbind(m_socket, 0);
}

CIP CSocketUDP::getAddress() const
{
	IPaddress *ip = SDLNet_UDP_GetPeerAddress(m_socket, 0);
	if (!ip)
		throw error::net((string)"Could not get address of UDP socket, SDL_Net error: " + SDLNet_GetError());
	CIP temp;
	temp.m_ip = *ip;
	return temp;
}


void CSocketUDP::send(const void *data, int nbytes)
{
	UDPpacket p;	//ugly
	//outcommented assignments are unneeded
	//p.channel	= 0;
	p.data		= reinterpret_cast<Uint8*>(const_cast<void*>(data));
	p.len		= nbytes;
	p.maxlen	= nbytes;
	p.status	= -1;
	//p.address	= *getAddress().getIPPointer();

	if (SDLNet_UDP_Send(m_socket, 0, &p) != 1)	//should have been sent to ONE place only
		throw error::net((string)"Could not send data, SDL_Net error: " + SDLNet_GetError());
}

void CSocketUDP::send(const CPacket& packet)
{
	Uint32 size = static_cast<Uint32>(packet.size());
	packet.putSizeFront();

	UDPpacket p;	//ugly
	memset(&p, 0, sizeof(p));
	//commented assignments are unneeded
	//p.channel	= 0;
	p.data		= const_cast<Uint8*>(packet.getDataPtr()-4);
	p.len		= size+4;
	p.maxlen	= size+4;
	p.status	= -1;
	//p.address	= *getAdress().getIPPointer();

	if (SDLNet_UDP_Send(m_socket, 0, &p) != 1)	//should have been sent to ONE place only
		throw error::net((string)"Could not send data, SDL_Net error: " + SDLNet_GetError());
}

uint CSocketUDP::recv(void *data, int maxlen)
{
	UDPpacket p;	//ugly again
	memset(&p, 0, sizeof(p));
	//commented assignments are unneeded
	//p.channel	= 0;
	p.data		= reinterpret_cast<Uint8*>(data);
	p.len		= maxlen;
	p.maxlen	= maxlen;
	p.status	= -1;
	//p.address	= *getAddress().getIPPointer();

	int ret = SDLNet_UDP_Recv(m_socket, &p);
	if (ret == 1)
		return p.len;
	else if (ret == 0)
		return 0;

	throw error::net_recv((string)"Could not receive packet, SDL_Net error: " + SDLNet_GetError());
}

bool CSocketUDP::recv(CPacket& packet)
{
	Uint32 size;
	if (recv(&size, sizeof(size)) != sizeof(size))
		return false;
	packet.allocate(size);
	if (recv(packet.getDataPtr(), size) != size)
		throw error::net((string)"Could only receive packet-size, but no data... BUFFER UDP AS WELL AS TCP!!1, SDL_Net error: " + SDLNet_GetError());
	return true;
}


bool CSocketUDP::ready()
{
	return (SDLNet_SocketReady(m_socket)!=0);
}

*/
