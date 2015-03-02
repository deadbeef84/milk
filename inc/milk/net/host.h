#ifndef MILK_NET_HOST_
#define MILK_NET_HOST_

#include "milk/net/net.h"
#include "milk/net/socket.h"
#include <set>

namespace milk
{
	/// abstract Host-class
	/**
		This is a Host-class for having IClients connecting to it.
		The clients are all numbered by a unique id, an int.
	*/
	class IHost : public ICounted
	{
	public:
		virtual ~IHost() { }

		/// sends a packet to a particular client
		virtual void send(int id, const CPacket& packet);

		/// sends a packet to all clients
		//virtual void sendAll(const CPacket& packet);

		/// non-blocking receive
		/**	returns true if there was something to receive.
			if id isn't 0, the sender of the packet received is put in *id */			
		virtual bool recv(CPacket& packet, int *id = 0);

		/// disconnects a client
		virtual void removeClient(int id) = 0;

		/// get a ISocket of a particual client
		/** throws invalid_argument if the client does not exist */
		virtual const ISocket& getClient(int id) const;

		/// is this a client?
		bool hasClient(int id) const;

		/// get a set containing all current clients
		std::set<int> getClientIDs() const;

		/// get the ip of this computer
		static std::string getLocalAddress();

	protected:
		typedef std::map<int, ISocket*> clientMap;
		typedef clientMap::iterator clientIterator;
		typedef clientMap::const_iterator constClientIterator;

		constClientIterator getClientPriv(int id) const;
		clientIterator getClientPriv(int id);

		clientMap m_clients;
		CSocketSet m_socketSet;
	};

	/// TCP-host class
	class CHostTCP : public IHost
	{
	public:
		CHostTCP();
		~CHostTCP();

		void open(int port);
		bool open() const;
		void close();

		/// listen for new clients
		/** Listens for new clients. if a new  client is connected,
			it is given the id specified by the argument and
			the call returns true.
			*/
		bool listen(int id);

		void removeClient(int id);

		const CSocketTCP& getClient(int id) const;

	private:
		void addClient(CSocketTCP*, int id);

		CSocketTCP m_listenSocket;
	};

	/*
	class CHostUDP : public IHost
	{
	public:
		CHostUDP();
		~CHostUDP();

		void addClient(const CIP& ip, Uint16 localPort, int id);
		void removeClient(int id);
		void removeAllClients();

		const CSocketUDP& getClient(int id) const;
	};
	*/

}

#endif
