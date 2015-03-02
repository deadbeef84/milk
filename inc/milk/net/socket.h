#ifndef MILK_NET_SOCKET_H_
#define MILK_NET_SOCKET_H_

#include "milk/net/net.h"
#include "milk/net/cpacket.h"

namespace milk
{
	/// abstract Socket-class
	class ISocket
	{
	public:
		ISocket() { }
		virtual ~ISocket() { }

		/// get the connected address
		virtual CIP getAddress() const=0;

		/// send some data
		/** if been disconnected, a error::net_disconnect is thrown.
			on other errors, a error::net_send is thrown.
		 */
		virtual void send(const void *data, uint nbytes)=0;

		/// send a packet
		/** if been disconnected, a error::net_disconnect is thrown.
			on other errors, a error::net_send is thrown.
		*/
		virtual void send(const CPacket& packet)=0;

		/// blocking receive. returns the nr of bytes read.
		/** if been disconnected, a error::net_disconnect is thrown.
		on other errors, a error::net_send is thrown.
		*/
		virtual uint recv(void *data, uint nbytes)=0;

		/// blocking receive of a packet.
		/**	Although this is a blocking receive, it may return false
			(indicating no packet was actually received)
			if only part of a packet was received.
			if been disconnected, a error::net_disconnect is thrown.
			on other errors, a error::net_send is thrown.
			*/
		virtual bool recv(CPacket& packet)=0;

	protected:
		int m_socket;

		vectorPOD<uchar> m_buffer;

	private:
		friend class CSocketSet;

		ISocket(const ISocket&);
		const ISocket& operator=(const ISocket&);
	};

	enum ConnectionStatus
	{
		DISCONNECTED,
		PENDING,
		CONNECTED
	};

	/// TCP-socket class
	class CSocketTCP : public ISocket
	{
	public:
		CSocketTCP();
		~CSocketTCP();
		
		/// opens a socket for listening.
		/** listen on the port specified in ip.
			becomes a server socket.
			throws a error::net on failure.
			*/
		void open(const CIP& ip);

		/// is this a open server-socket
		/** This call is invalid when called on a client-socket. */
		bool open() const;

		/// connect to an ip
		/*	connects to an ip.
			status will be set to PENDING.
			if the connection succeedes, it will
			then be set to CONNECTED, and if it fails,
			it will be (re)set to DISCONNECTED.
			if the connections succeeds, the
			socket becomes a client socket.
			the function throws a error::net on failure.
			*/
		void connect(const CIP& ip);

		/// what status is the connection?
		/** This call is invalid when called on a host-socket. */
		ConnectionStatus status();

		/// close both client and server sockets.
		/** Will wait out any connecting if one is
			in progress before disconnecting.
			This and status are the only legal calls
			when a connection is PENDING.
			*/
		void close();

		/// blocking accept. server sockets only.
		/** the client socket in the argument is
			where the new connected client goes.
			throws a error::net on failure.
			*/
		void accept(CSocketTCP& client);

		/// client-sockets only
		/** throws a error::net on failure.
		*/
		CIP getAddress() const;

		void send(const void *data, uint nbytes);
		void send(const CPacket& packet);

		uint recv(void *data, uint maxlen);
		bool recv(CPacket& packet);

		/// Turn Nagle algorithm on/off
		/**
			Nagle algorithm is a way to decrease traffic,
			since more data will be put into one ip-packet.
			It will increase delays, though, so Nagle algorithm
			might not be suitable for games.
			The Nagle algorithm is turned OFF by default.
			*/
		void setNagle(bool);
		/// is the Nagle algorithm turned on?
		bool getNagle()
		{ return m_nagle; }

	private:
		bool				m_open;
		bool				m_nagle;
		ConnectionStatus	m_status;
		CIP					m_remoteIP;
		CThread				m_connectThread;
		mutable CMutex		m_connectMutex;

		void setStatus(ConnectionStatus newStatus);

		// runs in different thread
		void connect_private();

		ConnectionStatus status_private() const;

		// buffers up a maximum of nbytes
		void recv_private(uint nbytes);

		CSocketTCP(const CSocketTCP&);
		const CSocketTCP& operator=(const CSocketTCP&);
	};

	/*
	class CSocketUDP : public ISocket
	{
	public:
		CSocketUDP();
		CSocketUDP(Uint16 port);	//for receiving
		~CSocketUDP();

		CIP getAddress() const;

		/*this is the local port on which to send from and receive to.
		0 will use any open port (recommended for clients,
		hosts can get the port when receiving anyway) *//*
		void open(Uint16 port);
		bool open() const;
		void close();

		/* the ip (and port) to which you are planning
		on sending messages to, and receiving from *//*
		void bind(const CIP& ip);
		void unbind();

		/* you must have both opened a port and
		bound a ip before sending or receiving *//*
		void send(const void *data, int nbytes);
		void send(const CPacket& packet);

		//both of thees are non-blocking, the first one will just return 0. none uses ready()
		uint recv(void *data, int maxlen);
		bool recv(CPacket& packet);

		/* don't really know why one would want to use this...
		recv is non-blocking anyway *//*
		bool ready();

		/* copying is forbidden, moving isn't *//*
		friend void swap(CSocketUDP& x, CSocketUDP& y)
		{ std::swap(x.m_socket, y.m_socket); }

	private:
		friend class CSocketSet;

		CSocketUDP(const CSocketUDP&);
		const CSocketUDP& operator=(const CSocketUDP&);
	};
	*/

}

#endif
