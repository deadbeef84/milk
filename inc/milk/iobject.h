#ifndef MILK_IOBJECT_H_
#define MILK_IOBJECT_H_

#include <list>

namespace milk
{
	class IObject
	{
	public:
		IObject()
			: m_userId(0), m_pUserData(0), m_refCount(0)
		{ }

		virtual ~IObject()
		{ }

		/// Increase reference counting
		/**
		This should be called whenever something depends on this node.
		In that way, that ensures this node doesn't get deleted.
		*/
		void addRef()
		{ ++m_refCount; }

		/// Get reference count.
		int getRefCount() const
		{ return m_refCount; }

		/// Decreases reference counting
		/**
		This should be called whenever something no longer depends on this node.
		When nothing depends on this node, it will be deleted.
		*/
		void release()
		{
			--m_refCount;
			if(m_refCount <= 0)
			{
				onDelete();
				delete this;
			}
		}

		int getUserId() const
		{ return m_userId; }

		void setUserId(int userId)
		{ m_userId = userId; }

		void* getUserData()
		{ return m_pUserData; }

		void setUserData(void *pUserData)
		{ m_pUserData = pUserData; }

		//virtual IObject* find(int userId)=0;



		void getReferences(std::list<IObject*> references)
		{ }

		int getReferenceCount()
		{ return 0; }

	protected:

		virtual void onDelete()
		{ }

		int m_userId;
		void *m_pUserData;
		int m_refCount;
	};

	template<class T>
	class handle
	{
	public:
		handle()
			: m_pObject(0)
		{ }

		handle(T *pObject)
			: m_pObject(0)
		{ set(pObject); }

		handle(const std::string& rid)
			: m_pObject(0)
		{ *this = rid; }

		handle(const handle<T>& c)
			: m_pObject(0)
		{ set(c.m_pObject); }

		~handle()
		{ set(0); }

		T* operator=(T *pObject)
		{
			set(pObject);
			return m_pObject;
		}

		T* operator=(handle<T> h)
		{
			set(h.m_pObject);
			return m_pObject;
		}

		T* operator=(const std::string& rid)
		{
			set(getResource<T>(rid));
			return m_pObject;
		}

		bool operator!() const
		{ return m_pObject?false:true; }

		operator bool() const
		{ return m_pObject?true:false; }

		bool operator &&(bool lhs) const
		{ return bool(*this) && lhs; }

		bool operator &&(const handle& lhs) const
		{ return bool(*this) && bool(lhs); }

		operator T*() const
		{ return m_pObject; }

		T& operator*() const
		{ return *m_pObject; }

		T* operator->() const
		{ return m_pObject; }

	private:
		void set(T *pObject)
		{
			if(m_pObject)
				safeRelease(m_pObject);
			m_pObject = pObject;
			if(m_pObject)
				m_pObject->addRef();
		}

		T* m_pObject;
	};
}

#endif
