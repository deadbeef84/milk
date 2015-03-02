#ifndef MILK_NET_PACKET_H_
#define MILK_NET_PACKET_H_

#include "milk/types.h"
#include "milk/math/cvector.h"
#include "milk/vectorpod.h"
#include <string>
#include <vector>

namespace milk
{
	/// container for easy packing/unpacking and sending/receiving of data
	/** When storing stuff in the packet, operator<< is used,
		and when reading from it, operator>>> is used.
		This still means everything is stored binary,
		in spite of obvious similarity with std::stream's.
		*/
	class CPacket
	{
	public:
		typedef unsigned short SizeHeader_t;

		CPacket() : m_data(sizeof(SizeHeader_t)), m_readpos(0) {}

		/// get the current size of the packet
		size_t size() const
		{ return m_data.size()-sizeof(SizeHeader_t); }

		/// change the size of the packet
		void resize(size_t size)
		{ m_data.resize(size+sizeof(SizeHeader_t)); }

		/// clear all contents
		void clear();

		/// puts the size of the packet before the data
		/** This may seem for a stupid function,
			but it useful for sending the whole
			packet in one call to send.
			example:

			CPacket p;
			p<<"something";
			p.putSizeFront();
			socket.send(p.getDataPtr()-sizeof(SizeHeader_t), p.size()+sizeof(SizeHeader_t));

			of course, socket.send(p) would also work,
			and is implemented somewhere the lines of the above.
			Note how you have to decrease the data pointer
			and increase the size, since the size is put BEFORE the packet itself.

			This is normally not something the user should use.
			*/
		void putSizeFront() const;

		/// returns a pointer to the contents of the packet
		uchar *getDataPtr()
		{ return &m_data[0]+sizeof(SizeHeader_t); }

		/// returns a constant pointer to the contents of the packet
		const uchar *getDataPtr() const
		{ return &m_data[0]+sizeof(SizeHeader_t); }

		/// append the data starting at [begin, begin+size)
		template<class It>
		void write(uint size, It begin)
		{
			for (; size!=0 ; --size, ++begin)
				*this << *begin;
		}

		/// append some data
		void write(const void *data, size_t nbytes);

#define __POUT(type)	CPacket& operator<<(type arg)\
						{ write(&arg, sizeof(arg)); return *this; }

		__POUT(char)
		__POUT(uchar)
		__POUT(signed char)

		__POUT(signed short)
		__POUT(unsigned short)

		__POUT(signed long)
		__POUT(unsigned long)

		//__POUT(Sint64)
		//__POUT(Uint64)

		__POUT(float)
		__POUT(double)

#undef __POUT

		CPacket& operator<<(const std::string& str);
		CPacket& operator<<(bool b)
		{ return (*this << (b?char(1):char(0))); }

		template<class T>
		CPacket& operator<<(const CVector3<T>& v)
		{ write(3, &v[0]); return *this; }

		template<class T>
		CPacket& operator<<(const CVector2<T>& v)
		{ write(2, &v[0]); return *this; }

		template<class T1, class T2>
		CPacket& operator<<(const std::pair<T1, T2>& p)
		{ return *this << p.first << p.second; }

		/////////////////////////////////////////////////

		/// set the position of where to read from
		void setReadPos(size_t pos) const
		{ m_readpos=pos; }

		/// get the position of where to read from
		size_t getReadPos() const
		{ return m_readpos; }

		/// read nbytes from the current readpos to data
		void read(void *data, uint nbytes) const;

		/// read a number of bytes from the current readpos to [begin, begin+size)
		template<class It>
		void read(uint size, It begin) const
		{
			for (; size!=0 ; --size, ++begin)
				*this >> *begin;
		}

#define __PIN(type)	const CPacket& operator>>(type& arg) const\
					{ read(&arg, sizeof(arg)); return *this; }

		__PIN(char)
		__PIN(uchar)
		__PIN(signed char)

		__PIN(signed short)
		__PIN(unsigned short)

		__PIN(signed long)
		__PIN(unsigned long)

		//__PIN(Sint64)
		//__PIN(Uint64)

		__PIN(float)
		__PIN(double)

#undef __PIN

		const CPacket& operator>>(std::string& str) const;
		const CPacket& operator>>(bool& b) const
		{ char t; *this >> t; b = t==0?false:true; return *this; }

		template<class T>
		const CPacket& operator>>(CVector3<T>& v) const
		{ read(3, &v[0]); return *this; }

		template<class T>
		const CPacket& operator>>(CVector2<T>& v) const
		{ read(2, &v[0]); return *this; }

		template<class T1, class T2>
		const CPacket& operator>>(std::pair<T1, T2>& p) const
		{ return *this >> p.first >> p.second; }

	private:
		mutable vectorPOD<uchar> m_data;
		mutable size_t m_readpos;
	};
}

#endif
