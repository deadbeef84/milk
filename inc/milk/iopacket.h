#ifndef MILK_IOPACKET_H_
#define MILK_IOPACKET_H_

#include "milk/net/cpacket.h"
#include "milk/math/cvector.h"
#include "milk/ccolor.h"

template<class F>
CPacket& operator<<(CPacket& p, const CColor<F>& color)
{ return p << color.r << color.g << color.b << color.a; }

template<class F>
const CPacket& operator>>(const CPacket& p, CColor<F>& color)
{ return p >> color.r >> color.g >> color.b >> color.a; }

template<class F>
CPacket& operator<<(CPacket& p, const CColor3<F>& color)
{ return p << color.r << color.g << color.b; }

template<class F>
const CPacket& operator>>(const CPacket& p, CColor3<F>& color)
{ return p >> color.r >> color.g >> color.b; }

#endif
