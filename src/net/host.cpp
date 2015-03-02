#include "milk/net/host.h"
#include "milk/helper.h"
using namespace milk;
using namespace std;


void IHost::send(int id, const CPacket& packet)
{
	BOOST_ASSERT(m_clients[id]);
	m_clients[id]->send(packet);
	addOut(packet.size()+sizeof(CPacket::SizeHeader_t));
}

/*
void IHost::sendAll(const CPacket& packet)
{
	for (clientIterator it = m_clients.begin() ; it != m_clients.end() ; ++it)
		it->second->send(packet);
	addOut((packet.size()+sizeof(Uint32)) * m_clients.size());
}
*/

bool IHost::recv(CPacket& packet, int *id)
{
	m_socketSet.check();

	for (clientIterator it = m_clients.begin() ; it!=m_clients.end() ; ++it)
	{
		if (m_socketSet.isSet(*it->second))
		{
			/*	set id first, since recv may throw an
				exception on disconnect, for instance. */
			if (id)
				*id = it->first;
			if (it->second->recv(packet))
			{
				addIn(packet.size()+sizeof(CPacket::SizeHeader_t));
				return true;
			}
		}
	}

	return false;
}

string IHost::getLocalAddress()
{
	char name[256];
	gethostname(name, 256);
	
	in_addr address;
	address.s_addr = inet_addr(name);
		
	hostent *hostEnt;
	
	if(address.s_addr == INADDR_NONE)
		hostEnt = gethostbyname(name);
	else
		hostEnt = gethostbyaddr(reinterpret_cast<const char *>(&address), sizeof(in_addr), AF_INET);

	if(hostEnt && hostEnt->h_addr_list[0])
	{
		// a host was found, return its ip
		in_addr *pAddr = reinterpret_cast<in_addr *>(hostEnt->h_addr_list[0]);
		return(inet_ntoa(*pAddr));
	}

	throw error::net("Could not find own ip");
}

bool IHost::hasClient(int id) const
{
	return (m_clients.count(id)==1);
}

std::set<int> IHost::getClientIDs() const
{
	std::set<int> clients;
	for (constClientIterator it = m_clients.begin() ; it!=m_clients.end() ; ++it)
		clients.insert(it->first);
	return clients;
}

const ISocket& IHost::getClient(int id) const
{
	return *getClientPriv(id)->second;
}

IHost::constClientIterator IHost::getClientPriv(int id) const
{
	constClientIterator it = m_clients.find(id);
	if (it == m_clients.end())
		throw invalid_argument("No such id \""+boost::lexical_cast<string>(id)+"\".");
	return it;
}

IHost::clientIterator IHost::getClientPriv(int id)
{
	clientIterator it = m_clients.find(id);
	if (it == m_clients.end())
		throw invalid_argument("No such id \""+boost::lexical_cast<string>(id)+"\".");
	return it;
}


///////////////////////////////////////////////7


CHostTCP::CHostTCP()
{ }

CHostTCP::~CHostTCP()
{ close(); }

void CHostTCP::open(int port)
{
	close();
	m_listenSocket.open(CIP(port));
}

bool CHostTCP::open() const
{
	return m_listenSocket.open();
}

void CHostTCP::close()
{
	if (open())
	{
		m_socketSet.clear();

		m_listenSocket.close();
		for (clientIterator it = m_clients.begin() ; it!=m_clients.end() ; ++it)
			delete it->second;

		m_clients.clear();
	}
}

void CHostTCP::addClient(CSocketTCP *sock, int id)
{
	m_clients[id] = sock;
	m_socketSet.add(*sock);
}

bool CHostTCP::listen(int id)
{
	CSocketSet acceptSet;
	acceptSet.add(m_listenSocket);
	acceptSet.check();
	if (acceptSet.isSet(m_listenSocket))
	{
		CSocketTCP *temp = new CSocketTCP();
		m_listenSocket.accept(*temp);
		addClient(temp, id);
		return true;
	}
	return false;
}

void CHostTCP::removeClient(int id)
{
	clientIterator it = getClientPriv(id);
	m_socketSet.remove(static_cast<CSocketTCP&>(*it->second));
	delete it->second;
	m_clients.erase(it);
}

const CSocketTCP& CHostTCP::getClient(int id) const
{
	return static_cast<const CSocketTCP&>(IHost::getClient(id));
}

///////////////////////////////////////////////7

/*
CHostUDP::CHostUDP()
{ }

CHostUDP::~CHostUDP()
{ removeAllClients(); }

void CHostUDP::addClient(const CIP& ip, Uint16 localPort, int id)
{
	BOOST_ASSERT(!hasClient(id));

	CSocketUDP *sock = new CSocketUDP();
	sock->open(localPort);
	sock->bind(ip);
	m_clients[id] = sock;
}

void CHostUDP::removeClient(int id)
{
	clientIterator it = getClientPriv(id);
	delete it->second;
	m_clients.erase(it);
}

void CHostUDP::removeAllClients()
{
	for (clientIterator it = m_clients.begin() ; it != m_clients.end() ; ++it)
		delete it->second;
	m_clients.clear();
}

const CSocketUDP& CHostUDP::getClient(int id) const
{
	return static_cast<const CSocketUDP&>(IHost::getClient(id));
}
*/
