#ifndef MILK_VECTORPOD_H_
#define MILK_VECTORPOD_H_

#include <stdexcept>
#include <memory>
#include <new>
#include <string>
#include "milk/helper.h"
#include "milk/boost.h"

namespace milk
{
	/// optimized version of std::vector, for for POD-types
	/**
		works just like std::vector (with some twists)
		but is highly optimized for POD-types.
		That is, types without copy-constructor,
		destructor or assignment-operator.
		vetorPOD guaranties none of the above to be called.
	*/
	template<class T, class A = std::allocator<T> >
	class vectorPOD
	{
	public:
		typedef A							allocator_type;
		typedef typename A::size_type		size_type;
		typedef typename A::difference_type	difference_type;
		typedef typename A::reference		reference;
		typedef typename A::const_reference	const_reference;
		typedef typename A::value_type		value_type;

		typedef typename A::pointer			iterator;
		typedef typename A::const_pointer	const_iterator;
		typedef std::reverse_iterator<iterator> reverse_iterator;
		typedef std::reverse_iterator<const_iterator> const_reverse_iterator;


		/* flawed: 0-pointers no good, can't deallocate them using allocator::deallocate
		explicit vectorPOD(const A& al = A())
		: m_first(0), m_last(0), m_end(0), m_allocator(al)
		{

		}
		*/

		explicit vectorPOD(const A& al = A())
		: m_allocator(al)
		{
			m_first = alloc(1);
			m_last = m_first;
			m_end = m_first+1;
		}

		explicit vectorPOD(size_type n, const T& value, const A& al = A())
			: m_allocator(al)
		{
			if (n!=0)
			{
				m_first = alloc(n);
				ufill(m_first, n, value);
				m_last = m_first + n;
				m_end = m_last;
			}
			else	// always a minimum of one element allocated
			{
				m_first = alloc(1);
				m_last = m_first;
				m_end = m_first + 1;
			}
		}

		//as above, but don't initialize elements
		explicit vectorPOD(size_type n, const A& al = A())
			: m_allocator(al)
		{
			if (n!=0)
			{
				m_first = alloc(n);
				m_last = m_first + n;
				m_end = m_last;
			}
			else	// always a minimum of one element allocated
			{
				m_first = alloc(1);
				m_last = m_first;
				m_end = m_first + 1;
			}
		}

		vectorPOD(const vectorPOD& v) : m_allocator(v.m_allocator)
		{
			if (!v.empty())
			{
				m_first = alloc(v.size());
				copy(m_first, v.m_first, v.size());
				m_last = m_first + v.size();
				m_end = m_last;
			}
			else	// always a minimum of one element allocated
			{
				m_first = alloc(1);
				m_last = m_first;
				m_end = m_first + 1;
			}
		}

		template<class IT>
		vectorPOD(IT first, IT last, const A& al = A())
			: m_allocator(al)
		{
			// always a minimum of one element allocated
			m_first = alloc(1);
			m_last = m_first;
			m_end = m_first+1;
			assign(first, last);
		}

		vectorPOD(const T* first, const T* last, const A& al = A())
			: m_allocator(al)
		{
			size_type n = last-first;
			if (n!=0)
			{
				m_first = alloc(n);
				copy(m_first, first, n);
				m_last = m_first+n;
				m_end = m_last;
			}
			else	// always a minimum of one element allocated
			{
				m_first = alloc(1);
				m_last = m_first;
				m_end = m_first + 1;
			}
		}

		~vectorPOD()
		{
			dealloc(m_first, capacity());
			m_first = m_last = m_end = 0;//reinterpret_cast<cosnt T*>(1337);	//maybe make debug only... dunno
		}

		vectorPOD& operator=(const vectorPOD& v)
		{
			if (this != &v)	// watch out for self-assignment
			{
				if (!v.empty())
				{
					if (capacity() <= v.size())
					{
						dealloc(m_first, capacity());
						m_first = alloc(v.size());
						m_end = m_first+v.size();
					}
					copy(m_first, v.m_first, v.size());
					m_last = m_first + v.size();
				}
				else
				{
					resize(0);
				}
			}
			return *this;
		}

		void reserve(size_type n)
		{
			if (capacity() < n)
			{
				size_type new_capacity = n;
				size_type old_size = size();

				T* new_first = alloc(new_capacity);
				copy(new_first, m_first, old_size);
				dealloc(m_first, capacity());

				m_first	= new_first;
				m_last	= m_first + old_size;
				m_end	= m_first + new_capacity;
			}
		}

	private:

		//private_reserves more than needed
		void private_reserve(size_type n)
		{
			if (capacity() < n)
			{
				size_type new_capacity = n*2;
				size_type old_size = size();

				T* new_first = alloc(new_capacity);
				copy(new_first, m_first, old_size);
				dealloc(m_first, capacity());

				m_first	= new_first;
				m_last	= m_first + old_size;
				m_end	= m_first + new_capacity;
			}
		}

	public:

		void resize(size_type n, const T& val)
		{
			private_reserve(n);
			if (size() < n)
				ufill(m_last, n-size(), val);
			m_last = m_first + n;
		}

		//as above, but don't initialize elements
		void resize(size_type n)
		{
			private_reserve(n);
			m_last = m_first + n;
		}

		size_type size() const
		{ return m_last-m_first; }

		size_type capacity() const
		{ return m_end-m_first; }

		bool empty() const
		{ return (m_first == m_last); }

		size_type max_size() const
		{ return m_allocator.max_size(); }

		const A& get_allocator() const
		{ return m_allocator; }


		iterator begin()
		{ return m_first; }
		const_iterator begin() const
		{ return m_first; }

		iterator end()
		{ return m_last; }
		const_iterator end() const
		{ return m_last; }

		reverse_iterator rbegin()
		{ return reverse_iterator(end()); }
		const_reverse_iterator rbegin() const
		{ return const_reverse_iterator(end()); }

		reverse_iterator rend()
		{ return reverse_iterator(begin()); }
		const_reverse_iterator rend() const
		{ return const_reverse_iterator(begin()); }


		reference at(size_type p)
		{
			if (size() <= p)
				throw std::out_of_range("invalid vectorPOD<T> subscript");
			return operator[](p);
		}
		const_reference at(size_type p) const
		{
			if (size() <= p)
				throw std::out_of_range("invalid vectorPOD<T> subscript");
			return operator[](p);
		}

		const_reference operator[](size_type p) const
		{ return *(m_first + p); }
		reference operator[](size_type p)
		{ return *(m_first + p); }

		reference front()
		{ return *m_first; }
		const_reference front() const
		{ return *m_first; }
		reference back()
		{ return *(m_last - 1); }
		const_reference back() const
		{ return *(m_last - 1); }


		void append(const T& v)
		{
			private_reserve(size()+1);
			copy(m_last, &v, 1);
			++m_last;
		}

		void append(size_type n, const T& v)
		{
			private_reserve(size()+n);
			ufill(m_last, n, v);
			m_last += n;
		}

		template<class IT>
		void append(IT first, IT last)
		{
			m_last = m_first;	//zero size
			for (; first!=last ; ++last)
				push_back(*last);
		}

		void append(const T* first, const T* last)
		{
			size_type n = last-first;
			private_reserve(size()+n);
			copy(m_last, first, n);
			m_last += n;
		}

		void push_back(const T& v)
		{
			append(v);
		}

		void push_back()
		{
			private_reserve(size()+1);
			++m_last;
		}

		void pop_back()
		{
			BOOST_ASSERT(!empty()); 
			--m_last;
		}


		template<class IT>
		void assign(IT first, IT last)
		{
			m_last = m_first;	//zero size
			append(first, last);
		}

		void assign(size_type n, const T& val)
		{
			m_last = m_first;	//zero size
			append(n, val);
		}


		iterator insert(iterator where, const T& v)
		{
			size_type pos = where-m_first;
			size_type ntrailing = m_last-where;
			resize(size()+1);
			where = m_first+pos;
			move(where+1, where, ntrailing);
			copy(where, &v, 1);
			return where+1;
		}

		iterator insert(iterator where)
		{
			size_type pos = where-m_first;
			size_type ntrailing = m_last-where;
			resize(size()+1);
			where = m_first+pos;
			move(where+1, where, ntrailing);
			return where+1;
		}

		iterator insert(iterator where, size_type n, const T& v)
		{
			size_type pos = where-m_first;
			size_type ntrailing = m_last - where;
			resize(size()+n);
			where = m_first+pos;
			move(where+n, where, ntrailing);
			ufill(where, n, v);
			return where+n;
		}

		iterator insert(iterator where, size_type n)
		{
			size_type pos = where-m_first;
			size_type ntrailing = m_last - where;
			resize(size()+n);
			where = m_first+pos;
			move(where+n, where, ntrailing);
			return where+n;
		}

		/*
		template<class IT>
		void insert(iterator where, IT first, IT last)
		{
		}

		void insert(iterator where, const T* first, const T* last)
		{
		}
		*/


		iterator erase(iterator where)
		{
			BOOST_ASSERT(m_first<=where && where<m_last); 
			//move [where+1, m_last) to [where, m_last-1)
			move(where, where+1, m_last-1-where);
			--m_last;
			return where;
		}

		/// move last element to the hole
		iterator erase_fast(iterator where)
		{
			BOOST_ASSERT(m_first<=where && where<m_last); 
			copy(where, m_last-1, 1);
			--m_last;
			return where;
		}

		/// erase [first, last)
		iterator erase(iterator first, iterator last)
		{
			BOOST_ASSERT(m_first<=first && first<=last && last<=m_last); 
			if (first != last)
			{
				move(first, last, m_last-last);
				m_last -= (last-first);
			}
			return first;
		}

		/// erase [first, last)
		iterator erase_fast(iterator first, iterator last)
		{
			BOOST_ASSERT(m_first<=first && first<=last && last<=m_last); 
			if (first != last)
			{
				size_type n = last-first;
				if (n < size_type(m_last-last))
					copy(first, m_last-n, n);
				else
					copy(first, last, m_last-last);
				m_last -= n;
			}
			return first;
		}

		void clear()
		{
			m_last = m_first;
		}

		void swap(vectorPOD& v)
		{
			if (m_allocator == v.m_allocator)
			{
				std::swap(m_first,	v.m_first);
				std::swap(m_last,	v.m_last);
				std::swap(m_end,	v.m_end);
			}
			else
			{
				vectorPOD temp(*this);
				*this = v;
				v = temp;
			}
		}

	private:
		static void ufill(iterator f, size_type n, const T& v)
		{
			for (; n != 0; --n, ++f)
				copy(f, &v, 1);
		}

		static void copy(T *to, const T *from, size_type n)
		{
			memcpy(to, from, n*sizeof(T));
		}

		static void move(T *to, const T *from, size_type n)
		{
			memmove(to, from, n*sizeof(T));
		}

		T *alloc(size_type n)
		{
			BOOST_ASSERT(n!=0);
			T* p = m_allocator.allocate(n, reinterpret_cast<const void*>(0));
			if (!p)
				throw std::bad_alloc();
			return p;
		}

		void dealloc(T* p, size_type n)
		{
			BOOST_ASSERT(p);
			m_allocator.deallocate(p, n);
		}

		iterator m_first, m_last, m_end;
		A m_allocator;
	};

	template<class T, class A>
	inline void swap(vectorPOD<T, A>& x, vectorPOD<T, A>& y)
	{ x.swap(y); }
}

#endif
