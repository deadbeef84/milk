#ifndef MILK_CDATACONTAINER_H_
#define MILK_CDATACONTAINER_H_

#include "milk/types.h"
#include <iterator>
#include <stdexcept>
#include <utility>
#include "milk/boost.h"

namespace milk
{
	template<class T>
	class CDataContainer
	{
	public:
		typedef size_t size_type;
		typedef ptrdiff_t _Dift;
		typedef _Dift difference_type;
		typedef T* _Tptr;
		typedef const T* _Ctptr;
		typedef _Tptr pointer;
		typedef _Ctptr const_pointer;
		typedef T& _Reft;
		typedef const T& _Creft;
		typedef _Reft reference;
		typedef _Creft const_reference;
		typedef T value_type;

		class const_iterator;
		friend class const_iterator;

		class const_iterator
			: public std::iterator<std::random_access_iterator_tag, T, _Dift, _Ctptr, _Creft>
		{
			typedef std::random_access_iterator_tag iterator_category;
			typedef T value_type;
			typedef _Dift difference_type;
			typedef _Ctptr pointer;
			typedef _Creft reference;

		public:
			const_iterator(const uchar* ptr, size_type stride = sizeof(value_type))
				: m_ptr(const_cast<uchar*>(ptr)), m_stride(stride)
			{ }

			const_iterator(_Ctptr* ptr, size_type stride = sizeof(value_type))
				: m_ptr(const_cast<uchar*>(reinterpret_cast<const uchar*>(ptr))), m_stride(stride)
			{ }

			const_reference operator*() const
			{
				return *reinterpret_cast<_Tptr>(m_ptr);
			}

			_Ctptr operator->() const
			{
				return const_cast<_Ctptr>(reinterpret_cast<_Tptr>(m_ptr));
			}

			const_iterator& operator ++()
			{
				m_ptr += m_stride;
				return *this;
			}

			const_iterator operator ++(int)
			{
				const_iterator tmp = *this;
				m_ptr += m_stride;
				return tmp;
			}

			const_iterator& operator --() 
			{
				m_ptr -= m_stride;
				return *this;
			}

			const_iterator operator --(int)
			{
				const_iterator tmp = *this;
				m_ptr -= m_stride;
				return tmp;
			}

			const_iterator& operator +=(difference_type n)
			{
				m_ptr += n*m_stride;
				return *this;
			}

			const_iterator operator +(difference_type n) const
			{
				return const_iterator(m_ptr+n*m_stride, m_stride);
			}

			const_iterator& operator -=(difference_type n)
			{
				m_ptr -= n*m_stride;
				return *this;
			}

			const_iterator operator -(difference_type n) const
			{
				return const_iterator(m_ptr-n*m_stride, m_stride);
			}

			difference_type operator -(const const_iterator& rhs) const
			{
				BOOST_ASSERT(m_stride == rhs.m_stride);
				return (m_ptr-rhs.m_ptr)/static_cast<difference_type>(m_stride);
			}

			const_reference operator[](difference_type off) const
			{ return (*(*this + off)); }

			bool operator==(const const_iterator& rhs) const
			{ return (m_ptr == rhs.m_ptr && m_stride == rhs.m_stride); }

			bool operator<(const const_iterator& rhs) const
			{
				BOOST_ASSERT(m_stride == rhs.m_stride);
				return m_ptr<rhs.m_ptr;
			}

			bool operator!=(const const_iterator& rhs) const
			{ return std::rel_ops::operator!=(*this,rhs); }

			bool operator>(const const_iterator& rhs) const
			{ return std::rel_ops::operator>(*this,rhs); }

			bool operator<=(const const_iterator& rhs) const
			{ return std::rel_ops::operator<=(*this,rhs); }

			bool operator>=(const const_iterator& rhs) const
			{ return std::rel_ops::operator>=(*this,rhs); }

			friend const_iterator operator+(difference_type off, const const_iterator& rhs)
			{ return (rhs + off); }

			uchar*	m_ptr;
			size_type	m_stride;
		};

		class iterator;
		friend class iterator;

		/// Iterator used to iterate through the interleaved data.
		/**
		This class is used by the IVertexBuffer class to enable iteration
		of interleaved arrays.
		*/
		class iterator
			: public const_iterator
		{
		public:
			typedef std::random_access_iterator_tag iterator_category;
			typedef T value_type;
			typedef _Dift difference_type;
			typedef _Tptr pointer;
			typedef _Reft reference;

			iterator(uchar* ptr, size_type stride = sizeof(value_type))
				: const_iterator(ptr, stride)
			{ }

			iterator(pointer ptr, size_type stride = sizeof(value_type))
				: const_iterator(reinterpret_cast<uchar*>(ptr), stride)
			{ }

			reference operator*() const
			{
				return ((reference)**(const_iterator *)this);
			}

			_Tptr operator->() const
			{
				return reinterpret_cast<_Tptr>(m_ptr);
			}

			iterator& operator ++()
			{
				m_ptr += m_stride;
				return *this;
			}

			iterator operator ++(int)
			{
				iterator tmp = *this;
				m_ptr += m_stride;
				return tmp;
			}

			iterator& operator --()
			{
				m_ptr -= m_stride;
				return *this;
			}

			iterator operator --(int)
			{
				iterator tmp = *this;
				m_ptr -= m_stride;
				return tmp;
			}

			iterator& operator +=(difference_type n)
			{
				m_ptr += n*m_stride;
				return *this;
			}

			iterator operator +(difference_type n) const
			{
				return iterator(m_ptr+n*m_stride, m_stride);
			}

			iterator& operator -=(difference_type n)
			{
				m_ptr -= n*m_stride;
				return *this;
			}

			iterator operator -(difference_type n) const
			{
				return iterator(m_ptr-n*m_stride, m_stride);
			}

			difference_type operator-(const iterator& rhs) const
			{
				BOOST_ASSERT(m_stride==rhs.m_stride);
				return (m_ptr-rhs.m_ptr)/static_cast<difference_type>(m_stride);
			}

			reference operator[](difference_type off) const
			{
				return *(*this + off);
			}

			friend iterator operator+(difference_type off, const iterator& rhs)
			{
				return (rhs + off);
			}
		};

		CDataContainer(T* first, size_type count, size_type stride = sizeof(T))
			: m_first(first), m_count(count), m_stride(stride)
		{
		}

		iterator begin()
		{ return iterator(reinterpret_cast<uchar*>(m_first), m_stride); }

		const_iterator begin() const
		{ return const_iterator(reinterpret_cast<const uchar*>(m_first), m_stride); }

		iterator end()
		{ return begin()+m_count; }

		const_iterator end() const
		{ return begin()+m_count; }

		reference operator [](size_type n)
		{ return *(begin()+n); }

		const_reference operator [](size_type n) const
		{ return *(begin()+n); }

		reference at(size_type pos)
		{
			if(size() <= pos)
				throw std::out_of_range("invalid CDataContainer<T> subscript");
			return *(begin()+pos);
		}

		const_reference at(size_type pos) const
		{
			if(size() <= pos)
				throw std::out_of_range("invalid CDataContainer<T> subscript");
			return *(begin()+pos);
		}

		reference front()
		{
			return *begin();
		}

		const_reference front() const
		{
			return *begin();
		}

		reference back()
		{
			return *(end() - 1);
		}

		const_reference back() const
		{
			return *(end() - 1);
		}

		bool empty() const
		{ return m_count==0; }

		size_type size() const
		{ return m_count; }

	private:
		T* m_first;
		size_type m_count;
		size_type m_stride;
	};
	// Force initialization... just to make sure it compiles
	template class CDataContainer<int>;
}

#endif
