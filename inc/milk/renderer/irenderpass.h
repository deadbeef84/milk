#ifndef MILK_IRENDERPASS_H_
#define MILK_IRENDERPASS_H_

#include "milk/helper.h"
#include "milk/renderer/irendertarget.h"
#include "milk/ccolor.h"
#include "milk/scenegraph/ccamera.h"
#include "milk/includes.h"
#include "milk/renderer/cmaterial.h"

namespace milk
{
	class CGeometry;
	class CPass;

	class IRenderPass
	{
		friend class CSceneManager;
	public:
		typedef std::pair<CGeometry*, CPass*> GeometryPass;
		typedef std::vector<GeometryPass> RenderQueue;

		enum ClearFlags
		{
			NOCLEAR=0,
			CLEARCOLOR=1,
			CLEARZ=2,
			CLEARSTENCIL=4
		};

		MILK_FIX_ENUM_FRIEND(IRenderPass::ClearFlags);

		//////////////////

		IRenderPass()
			: m_pRenderTarget(0), m_pCamera(0),
			  m_lastUpdate(0.0f), m_updateInterval(-1.0f),
			  m_clear(CLEARCOLOR|CLEARZ), m_clearColor(0.0f, 0.0f, 0.0f, 0.0f), m_clearDepth(1.0f),
			  m_lod(1.0f), m_passScope(0xffff)
		{ }

		IRenderPass(IRenderTarget *pRenderTarget, CCamera *pCamera)
			: m_pRenderTarget(0), m_pCamera(pCamera),
			  m_lastUpdate(0.0f), m_updateInterval(-1.0f),
			  m_clear(CLEARCOLOR|CLEARZ), m_clearColor(0.0f, 0.0f, 0.0f, 0.0f), m_clearDepth(1.0f),
			  m_lod(1.0f), m_passScope(0xffff)
		{ setRenderTarget(pRenderTarget); }

		virtual ~IRenderPass()
		{ }

		void setRenderTarget(IRenderTarget *pRenderTarget);
		IRenderTarget* getRenderTarget() const
		{ return m_pRenderTarget; }

		void setCamera(CCamera *pCamera)
		{ m_pCamera = pCamera; }
		CCamera* getCamera() const
		{ return m_pCamera; }

		void setViewport(const CRect<int>& viewportWindow)
		{ m_viewportWindow = viewportWindow; }
		const CRect<int>& getViewport() const
		{ return m_viewportWindow; }

		////////////////////////

		void setLod(float Lod)
		{ m_lod = Lod; }
		float getLod() const
		{ return m_lod; }

		void setPassScope(uint scope)
		{ m_passScope = scope; }
		uint getPassScope() const
		{ return m_passScope; }

		void setUpdateFrequency(float freq)
		{ m_updateInterval = 1.0f/freq; }

		////////////////////////

		void setClearFlags(ClearFlags flags)
		{ m_clear = flags; }
		ClearFlags getClearFlags() const
		{ return m_clear; }

		void setClearColor(CColor4f cc)
		{ m_clearColor = cc; }
		const CColor4f& setClearColor() const
		{ return m_clearColor; }

		void setClearDepth(float cd)
		{ m_clearDepth = cd; }
		float getClearDepth() const
		{ return m_clearDepth; }

		void clearRenderQueue()
		{ m_renderQueue.clear(); }
		void addToRenderQueue(CGeometry *pGeometry, CPass *pPass)
		{
			m_renderQueue.push_back(GeometryPass(pGeometry, pPass));
			std::push_heap(m_renderQueue.begin(), m_renderQueue.end(), sortRender);
		}
		void render();

	protected:
		bool beginRender();
		void endRender();

		void clear();

		static bool sortRender(const GeometryPass& a, const GeometryPass& b)
		{
			return CPass::sortPass(*a.second, *b.second);
		}

		///////////////

		handle<CCamera> m_pCamera;
		handle<IRenderTarget> m_pRenderTarget;

		float m_lod;
		uint m_passScope;

		// update frequency
		float m_lastUpdate;
		float m_updateInterval;

		// Viewport
		CRect<int> m_viewportWindow;

		////////////////////////////////////

		ClearFlags m_clear;
		CColor4f m_clearColor;
		float m_clearDepth;

		RenderQueue m_renderQueue;
	};
}

#endif
