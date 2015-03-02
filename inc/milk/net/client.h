#ifndef MILK_NET_CLIENT_H_
#define MILK_NET_CLIENT_H_

#include "milk/net/net.h"
#include "milk/net/socket.h"

namespace milk
{
	/// abstract Client class
	class IClient : public ICounted
	{
	public:
		virtual ~IClient() { }

		/// disconnect
		virtual void disconnect() = 0;

		/// send a packet to host
		virtual void send(const CPacket& packet);

		/// receive a packet
		/** This is a non-blocking call that
		returns true if a packet was indeed received. */
		virtual bool recv(CPacket& packet);

		/// get the ip of the host we are connected to
		std::string getRemoteIP() const;

	protected:
		CSocketSet m_socketSet;

		virtual ISocket& getSocket() =0;
		virtual const ISocket& getSocket() const =0;
	};

	/// TCP client
	class CClientTCP : public IClient
	{
	public:
		CClientTCP();
		~CClientTCP();

		/// see CSocketTCP::connect()
		void connect(const CIP& ip);

		/// see CSocketTCP::status()
		virtual ConnectionStatus status();

		/// see CSocketTCP::close()
		void disconnect();

	protected:
		ISocket& getSocket()
		{ return m_socket; }
		const ISocket& getSocket() const
		{ return m_socket; }

	private:
		CSocketTCP m_socket;
		bool m_isConnecting;
	};

	/*
	class CClientUDP : public IClient
	{
	public:
		CClientUDP();
		~CClientUDP();

		void connect(const CIP& ip, Uint16 listenPort);
		bool connected() const;
		void disconnect();

		bool recv(CPacket& packet);

	protected:
		ISocket& getSocket()
		{ return m_socket; }
		const ISocket& getSocket() const
		{ return m_socket; }

	private:
		CSocketUDP m_socket;
		bool m_connected;
	};
	*/
}

#endif
