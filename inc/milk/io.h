#ifndef MILK_IO_H_
#define MILK_IO_H_

#include "milk/math/cvector.h"
#include "milk/ccolor.h"
#include <iostream>

namespace milk
{

	namespace io
	{
		template<class T>
		inline void writepod(std::ostream& os, const T& t)
		{
			os.write((char*)&t, sizeof(T));
		}

		template<class T>
		inline T readpod(std::istream& is)
		{
			T t;
			is.read((char*)&t, sizeof(T));
			return t;
		}
	}
	
	// CVectorX //////////////////////////////////////////////////////////////

	template<class F>
	std::ostream& operator<<(std::ostream& o, const CVector4<F>& v)
	{
		o << '(' << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ')';
		return o;
	}

	template<class F>
	std::ostream& operator<<(std::ostream& o, const CVector3<F>& v)
	{
		o << '(' << v.x << ", " << v.y << ", " << v.z << ')';
		return o;
	}

	template<class F>
	std::ostream& operator<<(std::ostream& o, const CVector2<F>& v)
	{
		o << '(' << v.x << ", " << v.y << ')';
		return o;
	}

	//template<>
	inline std::ostream& operator<<(std::ostream& o, const CVector3<char>& v)
	{ return o<<CVector3<short>(v); }

	//template<>
	inline std::ostream& operator<<(std::ostream& o, const CVector3<unsigned char>& v)
	{ return o<<CVector3<short>(v); }

	//template<>
	inline std::ostream& operator<<(std::ostream& o, const CVector3<signed char>& v)
	{ return o<<CVector3<short>(v); }


	//template<>
	inline std::ostream& operator<<(std::ostream& o, const CVector2<char>& v)
	{ return o<<CVector2<short>(v); }

	//template<>
	inline std::ostream& operator<<(std::ostream& o, const CVector2<unsigned char>& v)
	{ return o<<CVector2<short>(v); }

	//template<>
	inline std::ostream& operator<<(std::ostream& o, const CVector2<signed char>& v)
	{ return o<<CVector2<short>(v); }

	template<class F>
	std::istream& operator>>(std::istream& i, CVector4<F>& v)
	{
		std::ios_base::fmtflags flags = i.flags();
		i.setf(std::ios::skipws);

		char t;
		i >> t;
		if (t!='(')
		{
			i.putback(t);
			i.setstate(std::ios_base::failbit);
		}
		i >> v.x >> t;
		if (t!=',')
		{
			i.putback(t);
			i.setstate(std::ios_base::failbit);
		}
		i >> v.y >> t;
		if (t!=',')
		{
			i.putback(t);
			i.setstate(std::ios_base::failbit);
		}
		i >> v.z >> t;
		if (t!=',')
		{
			i.putback(t);
			i.setstate(std::ios_base::failbit);
		}
		i >> v.w >> t;
		if (t!=')')
		{
			i.putback(t);
			i.setstate(std::ios_base::failbit);
		}

		i.flags(flags);

		//make sure eof is set if should
		i.peek();
		return i;
	}

	template<class F>
	std::istream& operator>>(std::istream& i, CVector3<F>& v)
	{
		std::ios_base::fmtflags flags = i.flags();
		i.setf(std::ios::skipws);

		char t;
		i >> t;
		if (t!='(')
		{
			i.putback(t);
			i.setstate(std::ios_base::failbit);
		}
		i >> v.x >> t;
		if (t!=',')
		{
			i.putback(t);
			i.setstate(std::ios_base::failbit);
		}
		i >> v.y >> t;
		if (t!=',')
		{
			i.putback(t);
			i.setstate(std::ios_base::failbit);
		}
		i >> v.z >> t;
		if (t!=')')
		{
			i.putback(t);
			i.setstate(std::ios_base::failbit);
		}

		i.flags(flags);

		//make sure eof is set if should
		i.peek();
		return i;
	}

	template<class F>
	std::istream& operator>>(std::istream& i, CVector2<F>& v)
	{
		std::ios_base::fmtflags flags = i.flags();
		i.setf(std::ios::skipws);

		char t;
		i >> t;
		if (t!='(')
		{
			i.putback(t);
			i.setstate(std::ios_base::failbit);
		}
		i >> v.x >> t;
		if (t!=',')
		{
			i.putback(t);
			i.setstate(std::ios_base::failbit);
		}
		i >> v.y >> t;
		if (t!=')')
		{
			i.putback(t);
			i.setstate(std::ios_base::failbit);
		}

		i.flags(flags);

		//make sure eof is set if should
		i.peek();

		return i;
	}

	//template<>
	inline std::istream& operator>>(std::istream& i, CVector3<char>& v)
	{ CVector3<short> temp; i>>temp; v = CVector3<char>(temp); return i; }

	//template<>
	inline std::istream& operator>>(std::istream& i, CVector3<unsigned char>& v)
	{ CVector3<short> temp; i>>temp; v = CVector3<unsigned char>(temp); return i; }

	//template<>
	inline std::istream& operator>>(std::istream& i, CVector3<signed char>& v)
	{ CVector3<short> temp; i>>temp; v = CVector3<signed char>(temp); return i; }


	//template<>
	inline std::istream& operator>>(std::istream& i, CVector2<char>& v)
	{ CVector2<short> temp; i>>temp; v = CVector2<char>(temp); return i; }

	//template<>
	inline std::istream& operator>>(std::istream& i, CVector2<unsigned char>& v)
	{ CVector2<short> temp; i>>temp; v = CVector2<unsigned char>(temp); return i; }

	//template<>
	inline std::istream& operator>>(std::istream& i, CVector2<signed char>& v)
	{ CVector2<short> temp; i>>temp; v = CVector2<signed char>(temp); return i; }

// CColor ////////////////////////////////////////////////////////////////

	template<class F>
	std::ostream& operator<<(std::ostream& o, const CColor4<F>& v)
	{
		o << '(' << v.r << ", " << v.g << ", " << v.b << ", " << v.a << ')';
		return o;
	}

	//template<>
	inline std::ostream& operator<<(std::ostream& o, const CColor4<char>& v)
	{ return o<<CColor4<short>(v); }

	//template<>
	inline std::ostream& operator<<(std::ostream& o, const CColor4<unsigned char>& v)
	{ return o<<CColor4<short>(v); }

	//template<>
	inline std::ostream& operator<<(std::ostream& o, const CColor4<signed char>& v)
	{ return o<<CColor4<short>(v); }

	template<class F>
	std::istream& operator>>(std::istream& i, CColor4<F>& v)
	{
		std::ios_base::fmtflags flags = i.flags();
		i.setf(std::ios::skipws);

		char t;
		i >> t;
		if (t!='(')
		{
			i.putback(t);
			i.setstate(std::ios_base::failbit);
		}
		i >> v.r >> t;
		if (t!=',')
		{
			i.putback(t);
			i.setstate(std::ios_base::failbit);
		}
		i >> v.g >> t;
		if (t!=',')
		{
			i.putback(t);
			i.setstate(std::ios_base::failbit);
		}
		i >> v.b >> t;
		if (t!=',')
		{
			i.putback(t);
			i.setstate(std::ios_base::failbit);
		}
		i >> v.a >> t;
		if (t!=')')
		{
			i.putback(t);
			i.setstate(std::ios_base::failbit);
		}

		//make sure eof is set if should
		i.peek();

		i.flags(flags);

		return i;
	}

	//template<>
	inline std::istream& operator>>(std::istream& i, CColor4<char>& v)
	{ CColor4<short> temp; i>>temp; v = CColor4<char>(temp); return i; }

	//template<>
	inline std::istream& operator>>(std::istream& i, CColor4<unsigned char>& v)
	{ CColor4<short> temp; i>>temp; v = CColor4<unsigned char>(temp); return i; }

	//template<>
	inline std::istream& operator>>(std::istream& i, CColor4<signed char>& v)
	{ CColor4<short> temp; i>>temp; v = CColor4<signed char>(temp); return i; }



	template<class F>
	std::ostream& operator<<(std::ostream& o, const CColor3<F>& v)
	{
		o << '(' << v.r << ", " << v.g << ", " << v.b << ')';
		return o;
	}

	//template<>
	inline std::ostream& operator<<(std::ostream& o, const CColor3<char>& v)
	{ return o<<CColor3<short>(v); }

	//template<>
	inline std::ostream& operator<<(std::ostream& o, const CColor3<unsigned char>& v)
	{ return o<<CColor3<short>(v); }

	//template<>
	inline std::ostream& operator<<(std::ostream& o, const CColor3<signed char>& v)
	{ return o<<CColor3<short>(v); }

	template<class F>
	std::istream& operator>>(std::istream& i, CColor3<F>& v)
	{
		std::ios_base::fmtflags flags = i.flags();
		i.setf(std::ios::skipws);

		char t;
		i >> t;
		if (t!='(')
		{
			i.putback(t);
			i.setstate(std::ios_base::failbit);
			return i;
		}
		i >> v.r >> t;
		if (t!=',')
		{
			i.putback(t);
			i.setstate(std::ios_base::failbit);
			return i;
		}
		i >> v.g >> t;
		if (t!=',')
		{
			i.putback(t);
			i.setstate(std::ios_base::failbit);
			return i;
		}
		i >> v.b >> t;
		if (t!=')')
		{
			i.putback(t);
			i.setstate(std::ios_base::failbit);
			return i;
		}

		//make sure eof is set if should
		i.peek();
		i.flags(flags);
		return i;
	}

	//template<>
	inline std::istream& operator>>(std::istream& i, CColor3<char>& v)
	{ CColor3<short> temp; i>>temp; v = CColor3<char>(temp); return i; }

	//template<>
	inline std::istream& operator>>(std::istream& i, CColor3<unsigned char>& v)
	{ CColor3<short> temp; i>>temp; v = CColor3<unsigned char>(temp); return i; }

	//template<>
	inline std::istream& operator>>(std::istream& i, CColor3<signed char>& v)
	{ CColor3<short> temp; i>>temp; v = CColor3<signed char>(temp); return i; }
}

#endif
