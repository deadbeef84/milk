#ifndef MILK_CPARTICLESYSTEM_H_
#define MILK_CPARTICLESYSTEM_H_

#include "milk/math/cvector.h"
#include "milk/math/cmatrix.h"
//#include "milk/cxmlparser.h"
#include "milk/vectorpod.h"
#include "milk/scenegraph/irenderable.h"
#include "milk/includes.h"
#include "milk/glhelper.h"
#include "milk/scenegraph/cscenemanager.h"
#include "milk/scenegraph/ccamera.h"
#include "milk/iresource.h"
#include <vector>
#include <map>

namespace milk
{
	class CParticleSystemBase : public ISceneNode
	{
	public:
		CParticleSystemBase()
		{
			//m_type = TYPE_3D;
		}
		virtual ~CParticleSystemBase() {}
	};


	template<class PARTICLE, class CONTAINER = vectorPOD<PARTICLE> >
	class CParticleSystem : public CParticleSystemBase
	{
	public:
		typedef CONTAINER containerType;
		typedef PARTICLE particleType;

		CParticleSystem(size_t numParticles)
			: m_particles(numParticles, PARTICLE()), m_relative(false), m_useColor(false), m_billboarded(true)
		{ }

		virtual ~CParticleSystem() {}

		virtual void beginRender() = 0;
		virtual void endRender() = 0;

		virtual void update(float dt) = 0;
		virtual void render();

	protected:
		CONTAINER m_particles;
		bool m_relative;
		bool m_useColor;
		bool m_billboarded;
	};

	//////////////////////////////////////////////////////////////////////////

	class ITexture; // FIXME: remove this when done with it...

	class CBasicParticle
	{
		friend class CBasicParticleSystem;
	public:
		CBasicParticle()
			: m_active(false)
		{ }

		inline bool active() const
		{ return m_active; }
		inline float size() const
		{ return m_size; }
		inline const CVector3f& position() const
		{ return m_position; }
		inline const CColor4f& color() const
		{ return CColor4f::RED; }
		inline const CVector3f& xAxis() const
		{ return CVector3f::XAXIS; }
		inline const CVector3f& yAxis() const
		{ return CVector3f::YAXIS; }

	private:
		bool m_active;
		float m_size;
		CVector3f m_position;
		CVector3f m_velocity;
		float m_life;
	};

	class CBasicParticleSystem : public CParticleSystem<CBasicParticle>
	{
	public:
		CBasicParticleSystem(size_t numParticles);
		virtual ~CBasicParticleSystem();

		virtual void update(float dt);
		virtual void beginRender();
		virtual void endRender();

	protected:
		CColor4f m_color;
		handle<ITexture> m_texture;
	};

	//////////////////////////////////////////////////////////////////////////

	class CAdvancedParticle
	{
		friend class CAdvancedParticleSystem;
	public:
		CAdvancedParticle()
			: m_active(false)
		{ }

		inline bool active() const
		{ return m_active; }
		inline float size() const
		{ return m_size; }
		inline const CVector3f& position() const
		{ return m_position; }
		inline const CColor4f& color() const
		{ return m_color; }
		inline const CVector3f& xAxis() const
		{ return m_xAxis; }
		inline const CVector3f& yAxis() const
		{ return m_yAxis; }

	private:
		bool m_active;
		CColor4f m_color;
		float m_startSize;
		float m_endSize;
		float m_size;
		CVector3f m_position;
		CVector3f m_velocity;
		float m_life;
		float m_maxLife;
		CVector3f m_xAxis;
		CVector3f m_yAxis;
	};
	
	class CAdvancedParticleSystem : public CParticleSystem<CAdvancedParticle>
	{
	public:
		CAdvancedParticleSystem(size_t numParticles, std::string filename);
		virtual ~CAdvancedParticleSystem();

		virtual void update(float dt);
		virtual void beginRender();
		virtual void endRender();

		void setBillboarding(bool bb)
		{ m_billboarded = bb; }

	//protected:
		CVector3f m_emitterSize;
		int m_emitCount;
		int m_emitCountBias;
		float m_emitGap;
		float m_emitGapBias;
		float m_life;
		float m_lifeBias;
		float m_initialVelocity;
		float m_initialVelocityBias;
		CVector3f m_initialDirection;
		CVector3f m_initialDirectionBias;
		CVector3f m_force;
		CColor4f m_startColor;
		CColor4f m_endColor;
		float m_startSize;
		float m_startSizeBias;
		float m_endSize;
		float m_endSizeBias;

		bool m_blend;
		GLenum m_blendSrc;
		GLenum m_blendDst;

		int m_numToEmit;
		bool m_autoRelease;

		size_t m_emitQueue;
		float m_emitTimer;
		float m_nextEmitTime;

		handle<ITexture> m_texture;
	};

	//////////////////////////////////////////////////////////////////////////

	class CAdvancedParticle2
	{
		friend class CAdvancedParticleSystem2;
	public:
		CAdvancedParticle2()
			: m_active(false)
		{ }

		inline bool active() const
		{ return m_active; }
		inline float size() const
		{ return m_size; }
		inline const CVector3f& position() const
		{ return m_position; }
		inline const CColor4f& color() const
		{ return m_color; }
		inline const CVector3f& xAxis() const
		{ return m_xAxis; }
		inline const CVector3f& yAxis() const
		{ return m_yAxis; }

	private:
		bool m_active;
		CColor4f m_color;
		float m_size;
		CVector3f m_position;
		CVector3f m_velocity;
		float m_life;
		float m_maxLife;
		CVector3f m_xAxis;
		CVector3f m_yAxis;
	};

	class CAdvancedParticleSystem2 : public CParticleSystem<CAdvancedParticle2>
	{
	public:
		CAdvancedParticleSystem2(size_t numParticles, std::string filename);
		virtual ~CAdvancedParticleSystem2();

		virtual void update(float dt);
		virtual void beginRender();
		virtual void endRender();

		void setBillboarding(bool bb)
		{ m_billboarded = bb; }

		//protected:
		CVector3f m_emitterSize;
		int m_emitCount;
		int m_emitCountBias;
		float m_emitGap;
		float m_emitGapBias;
		float m_life;
		float m_lifeBias;

		std::map<float, float> m_velocity;
		float m_velocityBias;

		CVector3f m_initialDirection;
		CVector3f m_initialDirectionBias;

		std::map<float, CColor4f> m_color;

		std::map<float, float> m_size;
		float m_sizeBias;

		bool m_blend;
		GLenum m_blendSrc;
		GLenum m_blendDst;

		int m_numToEmit;
		bool m_autoRelease;

		size_t m_emitQueue;
		float m_emitTimer;
		float m_nextEmitTime;

		handle<ITexture> m_texture;
	};

	//////////////////////////////////////////////////////////////////////////

	template<class P, class C>
	void CParticleSystem<P, C>::render()
	{
		beginRender();

		if(m_relative)
		{
			glPushMatrix();
			//doTransform();
		}

		//setWinding(m_relative?getTransformNode():0);

		// TODO: FIX WINDING!
		CMatrix4f mat = glGetMatrixf(GL_MODELVIEW_MATRIX);
		CVector3f right = mat.row3(0);
		CVector3f up = mat.row3(1);

		glBegin(GL_QUADS);
		for(C::iterator it = m_particles.begin(); it != m_particles.end(); ++it)
		{
			P& p = *it;
			if(p.active())
			{
				if(!m_billboarded)
				{
					right = p.xAxis();
					up = p.yAxis();
				}

				CVector3f vPoint0 = p.position() + ((-right - up) * p.size());
				CVector3f vPoint1 = p.position() + (( right - up) * p.size());
				CVector3f vPoint2 = p.position() + ((-right + up) * p.size());
				CVector3f vPoint3 = p.position() + (( right + up) * p.size());

				/*
					0 ~ 1
					|   |
					|   |
					2---3
				*/

				if(m_useColor)
					glColor4(p.color());

				//if(1/*getWinding()*/)
				{
					glTexCoord2f(1,1); glVertex3(vPoint0);
					glTexCoord2f(0,1); glVertex3(vPoint1);
					glTexCoord2f(0,0); glVertex3(vPoint3);
					glTexCoord2f(1,0); glVertex3(vPoint2);
				}
				/*else
				{
					glTexCoord2f(1,1); glVertex3(vPoint0);
					glTexCoord2f(1,0); glVertex3(vPoint2);
					glTexCoord2f(0,0); glVertex3(vPoint3);
					glTexCoord2f(0,1); glVertex3(vPoint1);
				}*/
			}
		}
		glEnd();
		
		if(m_relative)
			glPopMatrix();

		endRender();
	}

	//template class CParticleSystem<CParticleTest>;
}

#endif
