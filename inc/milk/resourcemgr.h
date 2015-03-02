#ifndef MILK_IRESOURCEMGR_H_
#define MILK_IRESOURCEMGR_H_

#include <map>
#include <string>

namespace milk
{
	class ITexture;

	template<class C>
	class ResourceMgr
	{
		friend class IResource;
	public:
		typedef typename std::map<std::string, C*> resourceList;

		static C* getResource(const std::string& resourceId)
		{
			resourceList::const_iterator ret = ms_resources.find(resourceId);
			if(ret == ms_resources.end())
			{
				// attempt to load resource
				C *pResource = C::create(resourceId);
				pResource->m_resourceId = resourceId;
				//pResource->loadResource(/* TODO */);
				ms_resources[resourceId] = pResource;
				return pResource;
			}
			return ret->second;
		}

		static void reload()
		{
			resourceList::iterator it = ms_resources.begin();
			for(; it != ms_resources.end(); ++it)
			{
				it->second->load(it->first);
			}
		}

		static size_t numResources()
		{
			return ms_resources.size();
		}

		static void setResourceId(C *pResource, const std::string& newResourceId)
		{
			if(pResource->m_resourceId == newResourceId)
				return;

			ms_resources.erase(pResource->m_resourceId);
			pResource->m_resourceId = newResourceId;
			if(!newResourceId.empty())
			{
				if(ms_resources.find(newResourceId) != ms_resources.end())
					throw error::milk("Resource name already exists");
				ms_resources[newResourceId] = pResource;
			}
		}

		static bool removeResource(const std::string& resourceId)
		{
			resourceList::iterator ret = ms_resources.find(resourceId);
			if(ret != ms_resources.end())
			{
				//IResource *pResource = ret->second;
				// pResource->m_refCount <-- TODO: Check refcounting?
				//delete pResource;
				ms_resources.erase(ret);
				return true;
			}
			return false;
		}

		static const resourceList& getResourceList()
		{ return ms_resources; }

	private:
		static resourceList ms_resources;

		// prevent creation
		ResourceMgr() { }
		~ResourceMgr() { }
	};

	template<class C>
	std::map<std::string, C*> ResourceMgr<C>::ms_resources;

	/// easier access to resources
	template<class T>
	inline T* getResource(const std::string& str)
	{
		return dynamic_cast<T*>(
			ResourceMgr<typename T::ResourceType>::getResource(str)
			);
	}
}

#endif
