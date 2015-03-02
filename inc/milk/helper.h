#ifndef MILK_HELPER_H_
#define MILK_HELPER_H_

#include <iostream>
#include <sstream>
#include <cctype>
#include <fstream>
#include <algorithm>
#include "milk/error.h"

/// Generate copy-ctor and assignment operator w/o implementation for class (ie. no copy)
#define MILK_NO_COPY(x)\
	x(x&);\
	x& operator=(x&);

/// Creates operators for enums
#define MILK_FIX_ENUM(TYPE)\
	inline TYPE operator|(TYPE a, TYPE b)\
	{ return static_cast<TYPE>(static_cast<int>(a)|static_cast<int>(b)); }\
	inline TYPE operator&(TYPE a, TYPE b)\
	{ return static_cast<TYPE>(static_cast<int>(a)&static_cast<int>(b)); }\
	inline TYPE operator^(TYPE a, TYPE b)\
	{ return static_cast<TYPE>(static_cast<int>(a)^static_cast<int>(b)); }

/// Creates operators for enums
#define MILK_FIX_ENUM_FRIEND(TYPE)\
	friend inline TYPE operator|(TYPE a, TYPE b)\
	{ return static_cast<TYPE>(static_cast<int>(a)|static_cast<int>(b)); }\
	friend inline TYPE operator&(TYPE a, TYPE b)\
	{ return static_cast<TYPE>(static_cast<int>(a)&static_cast<int>(b)); }\
	friend inline TYPE operator^(TYPE a, TYPE b)\
	{ return static_cast<TYPE>(static_cast<int>(a)^static_cast<int>(b)); }

namespace milk
{
	namespace text
	{
		inline bool isWhite(int c)
		{
			if (c>=0)
				return isspace(c)!=0;
			else
				return isspace(c+256)!=0;
		}

		inline int cmpNoCase(const std::string& str1, const std::string& str2)
		{
			std::string::const_iterator p = str1.begin(), p2 = str2.begin();
			while(p != str1.end() && p2 != str2.end())
			{
				if(toupper(*p) != toupper(*p2)) return (toupper(*p) < toupper(*p2)) ? -1 : 1;
				++p; ++p2;
			}
			return (str1.size() == str2.size()) ? 0 : (str1.size() < str2.size()) ? -1 : 1;
		}

		inline bool endsWith(const std::string& str, const std::string& end)
		{
			if(str.length() < end.length())
				return false;
			else if(str.length() == end.length())
				return str == end;
			else
				return str.substr(str.length()-end.length()) == end;
		}

		inline std::string strToUpper(std::string str)
		{
			std::transform(str.begin(), str.end(), str.begin(), ::toupper);
			return str;
		}

		inline std::string strToLower(std::string str)
		{
			std::transform(str.begin(), str.end(), str.begin(), ::tolower);
			return str;
		}

		inline std::string leftTrim(std::string str)
		{
			str.erase(0, str.find_first_not_of(" \n\r\t"));
			return str;
		}

		inline std::string rightTrim(std::string str)
		{
			str.resize(str.find_last_not_of(" \n\r\t") + 1);
			return str;
		}

		inline std::string trim(std::string str)
		{
			str.erase(0, str.find_first_not_of(" \n\r\t"));
			str.resize(str.find_last_not_of(" \n\r\t") + 1);
			return str;
		}

		inline std::string strReplace(const std::string& search, const std::string& replace, std::string text)
		{
			std::string::size_type pos = 0;
			while((pos = text.find(search, pos)) != std::string::npos)
			{
				text = text.substr(0, pos) + replace + text.substr(pos+search.length());
				pos += replace.length();
			}
			return text;
		}
	}

	template<class C>
	inline std::string toStr(const C& arg)
	{
		std::stringstream ss;
		ss << arg;
		if(ss.fail() || ss.bad())
			throw error::milk("toStr");
		return ss.str();
	}

	inline std::string toStr(const std::string& arg)
	{
		return arg;
	}

	inline std::string toStr(const char *arg)
	{
		return arg?arg:"";
	}

	inline std::string toStr(const size_t& arg)
	{
		return toStr<uint>((uint)arg);
	}

	// convert a string to a type using streams.
	template<class C>
	inline C fromStr(const std::string& str)
	{
		C x;
		std::stringstream ss(str);
		ss >> x;
		if(!ss.eof() || ss.fail() || ss.bad())
			throw error::milk("");
		return x;
	}

	inline bool fileExists(const std::string& fileName)
	{
		return std::ifstream(fileName.c_str()) ? true : false;
	}

	template<class T>
	void safeRelease(T* & pNode)
	{
		if(pNode)
			pNode->release();
		pNode = 0;
	}
	template<class T>
	void safeDelete(T* & pPointer)
	{
		if(pPointer)
			delete pPointer;
		pPointer = 0;
	}
	template<class T>
	void safeDeleteArray(T* & pPointer)
	{
		if(pPointer)
			delete [] pPointer;
		pPointer = 0;
	}

	template<class II>
	inline void delete_range(II first, II last)
	{
		for(; first != last; ++first)
		{
			if(*first)
			{
				delete *first;
				*first = 0;
			}
		}
	}

	template<class OI, class Sz>
	inline void delete_range_n(OI iter, Sz num)
	{
		for (; 0 < num; --num, ++iter)
		{
			if(*iter)
			{
				delete *iter;
				*iter = 0;
			}
		}
	}

	template<class II>
	inline void delete_range_array(II first, II last)
	{
		for(; first != last; ++first)
		{
			if(*first)
			{
				delete [] *first;
				*first = 0;
			}
		}
	}

	template<class OI, class Sz>
	inline void delete_range_array_n(OI iter, Sz num)
	{
		for (; 0 < num; --num, ++iter)
		{
			if(*iter)
			{
				delete [] *iter;
				*iter = 0;
			}
		}
	}
}

#endif
