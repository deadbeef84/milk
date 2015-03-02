#include "milk/net/cpacket.h"
#include <SDL.h>
#include <limits>

using namespace milk;
using namespace std;

void CPacket::clear()
{
	resize(0);
	setReadPos(0);
}

void CPacket::putSizeFront() const
{
	if (size() > numeric_limits<SizeHeader_t>::max())
		throw error::net_corrupt_packet("Too large packet!");

	SizeHeader_t s = static_cast<SizeHeader_t>(size());
	memcpy(&m_data[0], &s, sizeof(s));
}

void CPacket::write(const void *data, size_t nbytes)
{
	resize(size()+nbytes);
	uchar *end = getDataPtr() + size() - nbytes;
	memcpy(end, data, nbytes);
}

CPacket& CPacket::operator<<(const std::string& str)
{
	write(str.c_str(), str.size()+1);

	return *this;
}

void CPacket::read(void *data, size_t nbytes) const
{
	if (size() < getReadPos()+nbytes)
		throw error::net_corrupt_packet("Reading beyond the end of the packet");

	memcpy(data, getDataPtr()+m_readpos, nbytes);
	m_readpos += nbytes;
}

const CPacket& CPacket::operator>>(std::string& str) const
{
	str.clear();
	char temp;
	*this >> temp;
	while (temp != '\0')
	{
		str += temp;
		*this >> temp;
	}

	return *this;
}
