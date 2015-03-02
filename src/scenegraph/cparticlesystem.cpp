#include "milk/scenegraph/cparticlesystem.h"
#include "milk/renderer/ctexture.h"
#include "milk/cimage.h"
#include "milk/renderer.h"
using namespace milk;
using namespace std;

CBasicParticleSystem::CBasicParticleSystem(size_t numParticles)
	: CParticleSystem<particleType>(numParticles)
{
	m_texture = "data/gfx/particles/default.bmp";
}

CBasicParticleSystem::~CBasicParticleSystem()
{
}

void CBasicParticleSystem::beginRender()
{
	m_texture->bind();

	glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glColor4f(0.1f, 0.1f, 0.2f, 0.1f);
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);
	glBlendFunc(GL_DST_ALPHA, GL_ONE);
}

void CBasicParticleSystem::endRender()
{
	glPopAttrib();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void CBasicParticleSystem::update(float dt)
{
	// TODO
	//CMatrix4f m = m_relative ? CMatrix4f::IDENTITY : getTransform();
	CMatrix4f m = ltm();

	// Update active particles
	for(containerType::iterator it = m_particles.begin(); it != m_particles.end(); ++it)
	{
		particleType& p = *it;
		if(p.m_active)
		{
			p.m_velocity.y -= dt * 3.0f;
			p.m_position += p.m_velocity * dt;
			p.m_life -= dt;
			p.m_active = p.m_life > 0.0f;
			// bounce
			if(p.m_velocity.y < 0 && p.m_position.y < 30.0f)
				p.m_velocity.y = p.m_velocity.y * -0.6f;
		}
		else
		{
			// TODO/FIXME: what are these hard-coded values doing here?
			float ang = math::frand(0.0f, 2*math::PI_float);
			p.m_life = math::frand(4.0f, 9.0f);
			//p.m_size = math::frand(0.4f, 0.8f);
			p.m_size = math::frand(0.1f, 0.2f);

			p.m_position.set(0,0,0);
			p.m_position = m.transformPoint(p.m_position);
			p.m_velocity.set(math::cos(ang), -math::frand(4.0f, 6.0f), math::sin(ang));
			p.m_velocity = m.transformVector(p.m_velocity);

			p.m_active = true;
		}
	}

	// TODO
	//IRenderable::update(dt);
}

//////////////////////////////////////////////////////////////////////////

CAdvancedParticleSystem::CAdvancedParticleSystem(size_t numParticles, std::string filename)
: CParticleSystem<particleType>(numParticles),
  m_emitCount(1), m_emitCountBias(0),
  m_emitGap(1.0f), m_emitGapBias(1.0f),
  m_life(1.0f), m_lifeBias(0.0f),
  m_initialVelocity(1.0f), m_initialVelocityBias(0.0f),
  m_startSize(1.0f), m_startSizeBias(0.0f),
  m_endSize(1.0f), m_endSizeBias(0.0f),
  m_blend(true), m_blendSrc(GL_SRC_ALPHA), m_blendDst(GL_ONE_MINUS_SRC_ALPHA),
  m_numToEmit(-1), m_autoRelease(false),
  m_emitQueue(0), m_emitTimer(0.0f), m_nextEmitTime(0.0f)
{
	m_useColor = true;
	m_texture = filename;
}

CAdvancedParticleSystem::~CAdvancedParticleSystem()
{
}

void CAdvancedParticleSystem::beginRender()
{
	m_texture->bind();

	glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glDepthMask(GL_FALSE);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	if(m_blend)
	{
		glEnable(GL_BLEND);
		glBlendFunc(m_blendSrc, m_blendDst);
	}
}

void CAdvancedParticleSystem::endRender()
{
	glPopAttrib();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void CAdvancedParticleSystem::update(float dt)
{
	if(dt > 0)
	{
		m_emitTimer += dt;
		while(m_emitTimer >= m_nextEmitTime)
		{
			m_emitTimer -= m_nextEmitTime;
			m_nextEmitTime = m_emitGap + math::frand(-m_emitGapBias, m_emitGapBias);
			m_emitQueue += size_t(m_emitCount + math::irand(-m_emitCountBias, m_emitCountBias));
		}
	}

	// TODO
	//CMatrix4f m = m_relative ? CMatrix4f::IDENTITY : getTransform();
	CMatrix4f m = ltm();

	// Update active particles
	for(containerType::iterator it = m_particles.begin(); it != m_particles.end(); ++it)
	{
		particleType& p = *it;
		if(p.m_active)
		{
			float d = 1.0f - p.m_life / p.m_maxLife;
			p.m_size = math::lerp(d, p.m_startSize, p.m_endSize);
			p.m_color.r = math::lerp(d, m_startColor.r, m_endColor.r);
			p.m_color.g = math::lerp(d, m_startColor.g, m_endColor.g);
			p.m_color.b = math::lerp(d, m_startColor.b, m_endColor.b);
			p.m_color.a = math::lerp(d, m_startColor.a, m_endColor.a);
			p.m_velocity += dt * m_force;
			p.m_position += p.m_velocity * dt;
			p.m_life -= dt;
			p.m_active = p.m_life > 0.0f;
		}
		else if(m_emitQueue > 0 && (m_numToEmit > 0 || m_numToEmit == -1))
		{
			if(!m_billboarded)
			{
				p.m_xAxis = m.right();
				p.m_yAxis = m.at();
			}

			p.m_position = m.transformPoint(math::v3rand(-m_emitterSize, m_emitterSize));

			p.m_maxLife = p.m_life = m_life + math::frand(-m_lifeBias, m_lifeBias);
			p.m_size = p.m_startSize = m_startSize + math::frand(-m_startSizeBias, m_startSizeBias);
			p.m_endSize = m_endSize + math::frand(-m_startSizeBias, m_startSizeBias);

			CVector3f dir = m_initialDirection + math::v3rand(-m_initialDirectionBias, m_initialDirectionBias);
			if(dir.abs2())
				dir.normalize();
			dir *= m_initialVelocity + math::frand(-m_initialVelocityBias, m_initialVelocityBias);
			p.m_velocity = m.transformVector(dir);

			p.m_active = true;
			--m_emitQueue;

			if(m_numToEmit > 0)
				--m_numToEmit;
		}
	}

	// TODO
	//IRenderable::update(dt);

	if(m_numToEmit == 0 && m_autoRelease)
		release(); // FIXME: is this okay?
}

//////////////////////////////////////////////////////////////////////////

CAdvancedParticleSystem2::CAdvancedParticleSystem2(size_t numParticles, std::string filename)
: CParticleSystem<particleType>(numParticles),
m_emitCount(1), m_emitCountBias(0),
m_emitGap(1.0f), m_emitGapBias(1.0f),
m_life(1.0f), m_lifeBias(0.0f),
m_velocityBias(0.0f),
m_sizeBias(0.0f),
m_blend(true), m_blendSrc(GL_SRC_ALPHA), m_blendDst(GL_ONE_MINUS_SRC_ALPHA),
m_numToEmit(-1), m_autoRelease(false),
m_emitQueue(0), m_emitTimer(0.0f), m_nextEmitTime(0.0f)
{
	m_useColor = true;
	m_texture = filename;
}

CAdvancedParticleSystem2::~CAdvancedParticleSystem2()
{
}

void CAdvancedParticleSystem2::beginRender()
{
	m_texture->bind();

	glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glDepthMask(GL_FALSE);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	if(m_blend)
	{
		glEnable(GL_BLEND);
		glBlendFunc(m_blendSrc, m_blendDst);
	}
}

void CAdvancedParticleSystem2::endRender()
{
	glPopAttrib();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void CAdvancedParticleSystem2::update(float dt)
{
	if(dt > 0)
	{
		m_emitTimer += dt;
		while(m_emitTimer >= m_nextEmitTime)
		{
			m_emitTimer -= m_nextEmitTime;
			m_nextEmitTime = m_emitGap + math::frand(-m_emitGapBias, m_emitGapBias);
			m_emitQueue += size_t(m_emitCount + math::irand(-m_emitCountBias, m_emitCountBias));
		}
	}

	// TODO
	//CMatrix4f m = m_relative ? CMatrix4f::IDENTITY : getTransform();
	CMatrix4f m = ltm();

	// Update active particles
	for(containerType::iterator it = m_particles.begin(); it != m_particles.end(); ++it)
	{
		particleType& p = *it;
		if(p.m_active)
		{
			float d = 1.0f - p.m_life / p.m_maxLife;

			p.m_color = math::map_lerp(d, m_color);

			p.m_size = math::map_lerp(d, m_size);

			p.m_position += p.m_velocity * math::map_lerp(d, m_velocity) * dt;
			p.m_life -= dt;
			p.m_active = p.m_life > 0.0f;
		}
		else if(m_emitQueue > 0 && (m_numToEmit > 0 || m_numToEmit == -1))
		{
			if(!m_billboarded)
			{
				p.m_xAxis = m.right();
				p.m_yAxis = m.at();
			}

			p.m_position = m.transformPoint(math::v3rand(-m_emitterSize, m_emitterSize));

			p.m_maxLife = p.m_life = m_life + math::frand(-m_lifeBias, m_lifeBias);
			p.m_size = math::map_lerp(0.0f, m_size);

			CVector3f dir = m_initialDirection + math::v3rand(-m_initialDirectionBias, m_initialDirectionBias);
			if(dir.abs2())
				dir.normalize();
			p.m_velocity = m.transformVector(dir);

			p.m_active = true;
			--m_emitQueue;

			if(m_numToEmit > 0)
				--m_numToEmit;
		}
	}

	// TODO
	//IRenderable::update(dt);

	if(m_numToEmit == 0 && m_autoRelease)
		release(); // FIXME: is this okay?
}
