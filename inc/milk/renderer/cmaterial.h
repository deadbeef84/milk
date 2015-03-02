#ifndef MILK_CMATERIAL_H_
#define MILK_CMATERIAL_H_

#include "milk/ccolor.h"
#include "milk/renderer/ctexture.h"
#include "milk/renderer/ishader.h"
#include "milk/iresource.h"

namespace milk
{
	class IRenderPass;
	class CSceneManager;

	/// Specifies material related rendering states.
	class Material
	{
	public:
		Material()
			: m_ambient(0.2f, 0.2f, 0.2f, 0.0f),
			  m_diffuse(0.8f, 0.8f, 0.8f, 1.0f),
			  m_specular(0.0f, 0.0f, 0.0f, 0.0f),
			  m_emissive(0.0f, 0.0f, 0.0f, 0.0f),
			  m_shininess(0.0f),
			  m_lighting(true),
			  m_vertexColorTracking(false)
		{ }

		operator==(const Material& rhs) const
		{
			return m_ambient == rhs.m_ambient &&
				m_diffuse == rhs.m_diffuse &&
				m_specular == rhs.m_specular &&
				m_emissive == rhs.m_emissive &&
				m_shininess == rhs.m_shininess &&
				m_lighting == rhs.m_lighting &&
				m_vertexColorTracking == rhs.m_vertexColorTracking;
		}

		void setLighting(bool lighting)
		{ m_lighting = lighting; }
		bool getLighting() const
		{ return m_lighting; }

		void setVertexColorTracking(bool tracking)
		{ m_vertexColorTracking = tracking; }
		bool getVertexColorTracking() const
		{ return m_vertexColorTracking; }

		void setAmbient(const CColor4f& ambient)
		{ m_ambient = ambient; }
		const CColor4f& getAmbient() const
		{ return m_ambient; }

		void setDiffuse(const CColor4f& diffuse)
		{ m_diffuse = diffuse; }
		const CColor4f& getDiffuse() const
		{ return m_diffuse; }

		void setSpecular(const CColor4f& specular)
		{ m_specular = specular; }
		const CColor4f& getSpecular() const
		{ return m_specular; }

		void setEmissive(const CColor4f& emissive)
		{ m_emissive = emissive; }
		const CColor4f& getEmissive() const
		{ return m_emissive; }

		void setShininess(float shininess)
		{ m_shininess = shininess; }
		float getShininess() const
		{ return m_shininess; }

		void bind() const;

		static bool ms_vertexColor;

	private:
		CColor4f m_ambient, m_diffuse, m_specular, m_emissive;
		float m_shininess;
		bool m_lighting;
		bool m_vertexColorTracking;
	};

	/// Specifies polygon related rendering states.
	class PolygonMode
	{
	public:
		PolygonMode()
			: m_twoSidedLighting(false), m_localCameraLighting(false),
			  m_culling(GL_BACK), m_shading(GL_SMOOTH), m_winding(GL_CW),
			  m_rasterModeFront(GL_FILL), m_rasterModeBack(GL_FILL)
		{ }

		operator==(const PolygonMode& rhs) const
		{
			return m_twoSidedLighting == rhs.m_twoSidedLighting &&
				m_localCameraLighting == rhs.m_localCameraLighting &&
				m_culling == rhs.m_culling &&
				m_shading == rhs.m_shading &&
				m_winding == rhs.m_winding &&
				m_rasterModeFront == rhs.m_rasterModeFront &&
				m_rasterModeBack == rhs.m_rasterModeBack;
		}

		void setRasterMode(GLenum mode)
		{ m_rasterModeBack = m_rasterModeFront = mode; }

		void setRasterMode(GLenum front, GLenum back)
		{ m_rasterModeFront = front; m_rasterModeBack = back; }

		GLenum getRasterModeFront() const
		{ return m_rasterModeFront; }

		GLenum getRasterModeBack() const
		{ return m_rasterModeBack; }

		void setCulling(GLenum culling)
		{ m_culling = culling; }

		GLenum getCulling() const
		{ return m_culling; }

		void setShading(GLenum shading)
		{ m_shading = shading; }

		GLenum getShading() const
		{ return m_shading; }

		void setWinding(GLenum winding)
		{ m_winding = winding; }

		GLenum getWinding() const
		{ return m_winding; }

		void setTwoSidedLighting(bool twoSidedLighting)
		{ m_twoSidedLighting = twoSidedLighting; }

		bool getTwoSidedLighting() const
		{ return m_twoSidedLighting; }

		void setLocalCameraLighting(bool localCameraLighting)
		{ m_localCameraLighting = localCameraLighting; }

		bool getLocalCameraLighting() const
		{ return m_localCameraLighting; }

		void bind() const;

		GLenum getLightTarget() const
		{ return m_twoSidedLighting ? GL_FRONT_AND_BACK : GL_FRONT; }

		static GLenum ms_winding;
		static bool ms_swapWinding;
		static void setWindingGL(GLenum winding);

	protected:
		bool m_twoSidedLighting;
		bool m_localCameraLighting;
		GLenum m_culling;
		GLenum m_winding;
		GLenum m_shading;
		GLenum m_rasterModeFront, m_rasterModeBack;
	};

	/// 
	class CompositingMode
	{
	public:
		enum Blending
		{
			ALPHA,
			ALPHA_ADD,
			MODULATE,
			MODULATE_X2,
			REPLACE,
			OTHER
		};
		// TODO: Stencil, logical op, http://oss.sgi.com/projects/ogl-sample/registry/EXT/blend_equation_separate.txt

		CompositingMode()
			: m_depthFunc(GL_LEQUAL), m_depthTest(true), m_depthWrite(true),
			  m_alphaFunc(GL_GEQUAL), m_alphaRef(0), m_alphaTest(false), m_alphaWrite(true),
			  m_colorWrite(true), m_blendSrc(GL_ONE), m_blendDst(GL_ZERO),
			  m_depthOffsetFactor(0), m_depthOffsetUnits(0)
		{ }

		operator==(const CompositingMode& rhs) const
		{
			return m_depthFunc == rhs.m_depthFunc &&
				m_depthTest == rhs.m_depthTest &&
				m_depthWrite == rhs.m_depthWrite &&
				m_alphaFunc == rhs.m_alphaFunc &&
				m_alphaRef == rhs.m_alphaRef &&
				m_alphaTest == rhs.m_alphaTest &&
				m_alphaWrite == rhs.m_alphaWrite &&
				m_colorWrite == rhs.m_colorWrite &&
				m_blendSrc == rhs.m_blendSrc &&
				m_blendDst == rhs.m_blendDst &&
				m_depthOffsetFactor == rhs.m_depthOffsetFactor &&
				m_depthOffsetUnits == rhs.m_depthOffsetUnits;
		}

		void setDepthTest(bool depthTest)
		{ m_depthTest = depthTest; }

		bool getDepthTest() const
		{ return m_depthTest; }

		void setDepthWrite(bool depthWrite)
		{ m_depthWrite = depthWrite; }

		bool getDepthWrite() const
		{ return m_depthWrite; }

		void setColorWrite(bool colorWrite)
		{ m_colorWrite = colorWrite; }

		bool getColorWrite() const
		{ return m_colorWrite; }

		void setAlphaWrite(bool alphaWrite)
		{ m_alphaWrite = alphaWrite; }

		bool getAlphaWrite() const
		{ return m_alphaWrite; }

		void setBlending(Blending blending);

		void setBlending(GLenum blendSrc, GLenum blendDst)
		{ m_blendSrc = blendSrc; m_blendDst = blendDst; }

		Blending getBlending() const;

		GLenum getSourceBlend() const
		{ return m_blendSrc; }

		GLenum getDestinationBlend() const
		{ return m_blendDst; }

		void setAlphaThreshold(float alphaThreshold)
		{ m_alphaRef = alphaThreshold; }

		float getAlphaThreshold() const
		{ return m_alphaRef; }

		void setDepthOffsetFactor(float depthOffsetFactor)
		{ m_depthOffsetFactor = depthOffsetFactor; }

		float getDepthOffsetFactor() const
		{ return m_depthOffsetFactor; }

		void setDepthOffsetUnits(float depthOffsetUnits)
		{ m_depthOffsetUnits = depthOffsetUnits; }

		float getDepthOffsetUnits() const
		{ return m_depthOffsetUnits; }

		void bind() const;

	private:
		GLenum m_depthFunc;
		bool m_depthTest, m_depthWrite;

		GLenum m_alphaFunc;
		float m_alphaRef;
		bool m_alphaTest, m_alphaWrite;

		bool m_colorWrite;
		GLenum m_blendSrc, m_blendDst;
		
		float m_depthOffsetFactor, m_depthOffsetUnits;
	};

	template<class T>
	class Helper
	{
		typedef typename std::list<T> objList;
	public:
		static T* get(const T& obj)
		{
			if(obj == def)
				return 0;
			objList::iterator it = find(list.begin(), list.end(), obj);
			if(it != list.end())
				return &(*it);
			return &(*list.insert(list.end(), obj));
		}

		static void bind(T* pObj)
		{
			if(!pObj)
				pObj = &def;
			if(pObj != bound)
			{
				pObj->bind();
				bound = pObj;
			}
		}

		static const T& getObj(const T* pObj)
		{
			if(!pObj)
				pObj = &def;
			return *pObj;
		}

		static void unbind()
		{
			bound = 0;
		}

	private:
		static T* bound;
		static T def;
		static objList list;
	};
	template<class T>
	T* Helper<T>::bound = 0;

	template<class T>
	T Helper<T>::def;

	template<class T>
	typename Helper<T>::objList Helper<T>::list;

	///
	class CPass
	{
	public:
		CPass()
			: m_layer(0), m_passScope(0xffff),
			  m_pMaterial(0), m_pPolygonMode(0), m_pCompositingMode(0)
		{ }

		virtual ~CPass()
		{
			unloadShader();
		}

		static bool sortPass(const CPass& a, const CPass& b)
		{
			if(a.m_layer != b.m_layer)
				return a.m_layer < b.m_layer;

			if((CProgramObject*)a.m_programObject != (CProgramObject*)b.m_programObject)
				return (CProgramObject*)a.m_programObject < (CProgramObject*)b.m_programObject;

			for(int i=0; i<8; ++i)
				if(&(*a.m_textures[i]) != &(*b.m_textures[i]))
					return &(*a.m_textures[i]) < &(*b.m_textures[i]);

			if(a.m_pCompositingMode != b.m_pCompositingMode)
				return a.m_pCompositingMode < b.m_pCompositingMode;

			if(a.m_pPolygonMode != b.m_pPolygonMode)
				return a.m_pPolygonMode < b.m_pPolygonMode;

			return a.m_pMaterial < b.m_pMaterial;
		}

		void clearRenderPassDependencies()
		{ m_dependencies.clear(); }

		void addRenderpassDependency(IRenderPass *pRenderpass)
		{ m_dependencies.push_back(pRenderpass); }

		void setLayer(int layer)
		{ m_layer = layer; }
		int getLayer() const
		{ return m_layer; }

		void setPassScope(uint scope)
		{ m_passScope = scope; }
		uint getPassScope() const
		{ return m_passScope; }

		void setTexture(uint unit, ITexture* pTexture)
		{ m_textures[unit] = pTexture; }
		ITexture* getTexture(uint unit)
		{ return m_textures[unit]; }

		void setMaterial(const Material& material)
		{
			m_pMaterial = Helper<Material>::get(material);
		}

		const Material& getMaterial() const
		{
			return Helper<Material>::getObj(m_pMaterial);
		}

		void setPolygonMode(const PolygonMode& polygonMode)
		{
			m_pPolygonMode = Helper<PolygonMode>::get(polygonMode);
		}

		const PolygonMode& getPolygonMode() const
		{
			return Helper<PolygonMode>::getObj(m_pPolygonMode);
		}

		void setCompositingMode(const CompositingMode& compositingMode)
		{
			m_pCompositingMode = Helper<CompositingMode>::get(compositingMode);
		}

		const CompositingMode& getCompositingMode() const
		{
			return Helper<CompositingMode>::getObj(m_pCompositingMode);
		}

		virtual void bind();

		void unloadShader();
		void loadShader(const std::string& vfilename, const std::string& ffilename);
		void loadShader(const std::string& filename)
		{ loadShader(filename+".vert", filename+".frag"); }

		CProgramObject* getProgramObject()
		{ return m_programObject; }

	protected:
		handle<ITexture> m_textures[8];
		handle<CProgramObject> m_programObject;
		handle<CVertexShader> m_vertexShader;
		handle<CFragmentShader> m_fragmentShader;

		int m_layer;
		uint m_passScope;

		Material *m_pMaterial;
		PolygonMode *m_pPolygonMode;
		CompositingMode *m_pCompositingMode;

		std::vector<IRenderPass*> m_dependencies;
	};
}

#endif
