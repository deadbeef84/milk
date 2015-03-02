#include "milk/net/client.h"
using namespace milk;
using namespace std;

void IClient::send(const CPacket& packet)
{
	getSocket().send(packet);
	addOut(packet.size()+sizeof(CPacket::SizeHeader_t));
}

bool IClient::recv(CPacket& packet)
{
	m_socketSet.check();
	if (m_socketSet.isSet(getSocket()) && getSocket().recv(packet))
	{
		addIn(packet.size()+sizeof(CPacket::SizeHeader_t));
		return true;
	}
	return false;
}

string IClient::getRemoteIP() const
{
	return getSocket().getAddress().getIPAndPort();
}

/////////////////////////////////////////////


CClientTCP::CClientTCP() : m_isConnecting(false)
{
}

CClientTCP::~CClientTCP()
{
	disconnect();
}

void CClientTCP::connect(const CIP& ip)
{
	disconnect();
	m_isConnecting = true;
	m_socket.connect(ip);
}

ConnectionStatus CClientTCP::status()
{
	ConnectionStatus status = m_socket.status();
	// done connecting
	if (m_isConnecting && status!=PENDING)
	{
		if (status==CONNECTED)
			m_socketSet.add(m_socket);
		m_isConnecting = false;
	}
	return status;
}

void CClientTCP::disconnect()
{
	if (status()!=DISCONNECTED)
	{
		m_socket.close();
		m_socketSet.remove(m_socket);
	}
}


/////////////////////////////////////////////

/*
CClientUDP::CClientUDP()
	: m_connected(false)
{ }

CClientUDP::~CClientUDP()
{
	disconnect();
}

void CClientUDP::connect(const CIP& ip, Uint16 listenPort)
{
	disconnect();
	m_socket.open(listenPort);
	m_socket.bind(ip);
	m_connected = true;
}

void CClientUDP::disconnect()
{
	if (connected())
	{
		m_socket.unbind();
		m_socket.close();
		m_connected = false;
	}
}

bool CClientUDP::connected() const
{
	return m_connected;
}


bool CClientUDP::recv(CPacket& packet)
{
	if (m_connected)
		return IClient::recv(packet);
	else
		return false;
}

*/
