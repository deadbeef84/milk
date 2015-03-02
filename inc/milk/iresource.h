#ifndef MILK_IRESOURCE_H_
#define MILK_IRESOURCE_H_

#include "milk/types.h"
#include "milk/resourcemgr.h"
#include "milk/helper.h"
#include "milk/iobject.h"

namespace milk
{

#define MILK_MAKE_RESOURCE(x)\
	typedef x ResourceType;\
	void setResourceId(const std::string& resourceId)\
	{\
		ResourceMgr<x::ResourceType>::setResourceId(this, resourceId);\
	}\
	bool removeResource()\
	{\
		return ResourceMgr<x::ResourceType>::removeResource(getResourceId());\
	}

	class IResource : public IObject
	{
		template <class>
		friend class ResourceMgr;

	public:
		typedef IResource ResourceType;

		IResource()
		{
			++ms_resourceCount;
		}

		virtual ~IResource()
		{
			--ms_resourceCount;
		}

		/// Total number of allocated IResource's.
		static size_t numResources()
		{ return ms_resourceCount; }

		// TODO
		//virtual void clone()=0;

		virtual void setResourceId(const std::string& resourceId)=0;
		virtual bool removeResource()=0;

		const std::string& getResourceId() const
		{ return m_resourceId; }

	protected:
		virtual void onDelete()
		{
			removeResource();
			IObject::onDelete();
		}

	private:
		std::string m_resourceId;
		static size_t ms_resourceCount;
	};
}

#endif
