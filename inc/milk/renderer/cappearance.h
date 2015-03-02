#ifndef MILK_CAPPEARANCE_H_
#define MILK_CAPPEARANCE_H_

#include "milk/renderer/irenderpass.h"
#include "milk/renderer/cmaterial.h"
#include "milk/iresource.h"
#include <vector>

namespace milk
{
	class CSceneManager;

	/// Defines the appearance of geometry.
	class CAppearance : public IResource
	{
		friend class CSceneManager;
	public:
		CAppearance()
		{
			m_passes.push_back(new CPass());
		}

		virtual ~CAppearance()
		{
			delete_range(m_passes.begin(), m_passes.end());
		}
		
		MILK_MAKE_RESOURCE(CAppearance);

		// FIXME - TODO: Create material from file
		static CAppearance* create(const std::string&)
		{ return new CAppearance(); }

		CPass& getPass(uint num)
		{ return *m_passes[num]; }

		virtual void prepareRenderPass(CSceneManager *, IRenderPass *)
		{ }

		virtual void setupRenderPass(CSceneManager *, IRenderPass *)
		{ }

		void prepareGeometry(CSceneManager *pSceneManager, CGeometry *pGeometry) const;

		static CAppearance& getDefault()
		{ return *ms_pDefault; }

		static void init()
		{
			ms_pDefault = new CAppearance();
			ms_pDefault->addRef();
		}

		static void free()
		{
			safeRelease(ms_pDefault);
		}

	protected:
		IRenderPass *m_pRenderPass;
		static CAppearance *ms_pDefault;
		typedef std::vector<CPass*> passList;
		passList m_passes;
	};
}

#endif
