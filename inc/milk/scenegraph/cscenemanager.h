#ifndef MILK_CSCENEMANAGER_H_
#define MILK_CSCENEMANAGER_H_

#include "milk/scenegraph/iscenenode.h"
#include "milk/scenegraph/ccamera.h"
#include "milk/renderer/iindexbuffer.h"
#include "milk/renderer/ivertexbuffer.h"
#include "milk/renderer/cgeometry.h"
#include <queue>
#include <vector>
#include <algorithm>
#include <map>

namespace milk
{
	class CLight;
	class CClipPlane;

	class IWindow;
	class IRenderPass;
	class CAppearance;
	class CPass;
	class CGeometry;

	/// Manages a scene. This should be the root node of the scene.
	class CSceneManager : public ISceneNode
	{
		friend class CLight;
		friend class CClipPlane;
		friend class IRenderPass;

		class CameraRenderInfo {
		public:
			CameraRenderInfo()
				: calculated(false)
			{ }

			void clear()
			{
				calculated = false;
				geometry.clear();
				//clipPlanes.clear();
				//lights.clear();
			}

			bool calculated;
			std::vector<CGeometry> geometry;
			//std::vector<CClipPlane*> clipPlanes;
			//std::vector<CLight*> lights;
		};

	public:
		CSceneManager();

		virtual ~CSceneManager();

		void render();
		void render(IRenderPass *pRenderPass);
		void render(const CGeometry& geometry);

		IRenderPass *getActiveRenderPass()
		{ return m_pActiveRenderPass; }

		CCamera *getActiveCamera()
		{ return m_pActiveCamera; }

		std::vector<CLight*>& getLights()
		{ return m_lights; }
		std::vector<CClipPlane*>& getClipPlanes()
		{ return m_clipPlanes; }

		void disableFog()
		{ m_fogMode = GL_NONE; }
		void setFogLinear(const CColor4f& color, GLfloat start, GLfloat end)
		{ m_fogMode = GL_LINEAR; m_fogColor = color; m_fogArg1 = start; m_fogArg2 = end; }
		void setFogExponenial(const CColor4f& color, GLfloat density)
		{ m_fogMode = GL_EXP; m_fogColor = color; m_fogArg1 = density; }
		void setFogExponenial2(const CColor4f& color, GLfloat density)
		{ m_fogMode = GL_EXP2; m_fogColor = color; m_fogArg1 = density; }

		void setStates();

#ifndef NDEBUG
		static CSceneManager* getDebugSceneManager()
		{ return ms_sceneManagers.empty()?0:ms_sceneManagers.front(); }
#endif

	protected:
		// Clip planes
		void addClipPlane(CClipPlane *pClipPlane)
		{
			m_clipPlanes.push_back(pClipPlane);
		}

		void removeClipPlane(CClipPlane *pClipPlane)
		{
			std::vector<CClipPlane*>::iterator it = std::find(m_clipPlanes.begin(), m_clipPlanes.end(), pClipPlane);
			if(it != m_clipPlanes.end())
				m_clipPlanes.erase(it);
		}

		// Lights
		void addLight(CLight *pLight)
		{
			m_lights.push_back(pLight);
		}

		void removeLight(CLight *pLight)
		{
			std::vector<CLight*>::iterator it = std::find(m_lights.begin(), m_lights.end(), pLight);
			if(it != m_lights.end())
				m_lights.erase(it);
		}

		IRenderPass *m_pActiveRenderPass;
		CCamera *m_pActiveCamera;
		IWindow *m_pActiveWindow;

		void setupClipPlanes();
		void setupLights();

	private:
		// states
		GLenum m_fogMode;
		CColor4f m_fogColor;
		GLfloat m_fogArg1, m_fogArg2;

		// ...
		std::vector<CLight*> m_lights;
		std::vector<CClipPlane*> m_clipPlanes;

		std::vector<IRenderPass*> m_renderPasses;

		#ifndef NDEBUG
		static std::vector<CSceneManager*> ms_sceneManagers;
		#endif
		
		typedef std::map<CCamera*, CameraRenderInfo> cameraMap;
		cameraMap m_cameraRenderInfo;
		CameraRenderInfo *m_pCameraRenderInfo;
	};
}

#endif
