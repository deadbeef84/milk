#ifndef MILK_NET_H_
#define MILK_NET_H_

#include "milk/includes.h"
#include "milk/obsolete/milk_cthread.h"
#include "milk/helper.h"
#include "milk/math/cvector.h"
#include "milk/timer.h"
#include "milk/types.h"
#include "milk/vectorpod.h"
#include "errno.h"
#include <map>
#include <utility>
#include <string>
#include <queue>
#include <set>
#include "milk/boost.h"

namespace milk
{
	/// net-class. only one public member, a function for getting possible error
	class Net
	{
	private:
		static std::string errorStr()
		{
			std::string ret = ", error nr: ";
#ifdef WIN32
			ret += boost::lexical_cast<std::string>(WSAGetLastError());
			WSASetLastError(0);
#else
			ret += boost::lexical_cast<std::string>(errno);
			errno = 0;
#endif
			return ret;
		}

		friend class CSocketTCP;
		friend class CIP;
		friend class CSocketSet;

		Net();
		friend class IApplication;
		static void init();
		static void free();
	};

	/// IP-class
	class CIP
	{
	public:
		CIP(){}

		/// open a server-ip
		explicit CIP(int port);

		/// resolve hostname and make this a client-IP
		explicit CIP(const std::string& hostname, int port);

		/// open a server-ip
		void server(int port);

		/// resolve hostname and make this a client-IP
		void resolve(const std::string& hostname, int port);

		/// get the ip (in local uchar order)
		Uint32 getIP() const;

		/// get the port
		Uint16 getPort() const;

		/// get the ip as a string (in the format "127.0.0.1")
		std::string getIPAsString() const;

		/// get the ip and port as a string (in the format "127.0.0.1:1337")
		std::string getIPAndPort() const;

	protected:
		const sockaddr_in *getAddress() const
		{ return &m_ip; }

		sockaddr_in *getAddress()
		{ return &m_ip; }

	private:
		sockaddr_in m_ip;

		friend class CSocketTCP;
		//friend class CSocketUDP;
	};

	class ISocket;

	/// Socket-set for checking sockets for activity
	/**	this is NOT a container for sockets.
		it is a class for determining wheter
		its members has something to receive,
		someone to accept or has been disconnected.	*/
	class CSocketSet
	{
	public:
		CSocketSet();
		~CSocketSet();

		/**	this checks if the members are ready to receive,
			ready to accept or has been disconnected.
			use isSet after calling this to see if a certain socket is ready.
			timeout is in milliseconds. */
		void check(int timeout=0);

		/// add a socket to the set
		void add(const ISocket&);

		/// remove a socket from the set
		void remove(const ISocket&);

		/// remove all sockets from the set
		void clear();

		/// do we have this socket?
		bool contains(const ISocket&);

		/// is this socket ready for receive or accept, or has this socket been disconnected?
		bool isSet(const ISocket&);	

	private:
		std::set<int> m_set;	//this is all the sockets we are watching
		fd_set m_in;			//this is the sockets with activity pending
	};

	/// base for IClient and IHost, used for counting sent/received
	class ICounted
	{
	public:
		ICounted();
		virtual ~ICounted() { };

		/// get the bytes/second received
		double getInRate() const;

		/// get the bytes/second sent
		double getOutRate() const;

	protected:
		void addOut(size_t nbytes);
		void addIn(size_t nbytes);

	private:
		void updateRate() const;

		mutable size_t m_outBytes, m_inBytes;
		mutable double m_inRate, m_outRate;
		mutable CTimer m_rateTimer;
	};
}

#endif
