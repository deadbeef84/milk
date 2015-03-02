#include "milk/audio/csoundsource.h"
#include "milk/audio/isound.h"
#include "milk/audio/sound.h"
#include "milk/audio/ceffectgroup.h"
#include "milk/error.h"
#include "milk/helper.h"
#include "milk/vectorPOD.h"
#include "milk/boost.h"
using namespace milk;
using namespace std;

CSoundSource::CSoundSource()
: m_sourceId(0), m_pSound(0), m_pControl(0)
{
	alGenSources(1, &m_sourceId);
}

CSoundSource::CSoundSource(ISound* pSound)
: m_sourceId(0), m_pSound(0), m_pControl(0)
{
	alGenSources(1, &m_sourceId);
	setSound(pSound);
}

CSoundSource::~CSoundSource()
{
	setSound(0);
	disableDirectFilter();
	disableAuxSend(0);
	alDeleteSources(1, &m_sourceId);
}

void CSoundSource::play()
{
	m_pControl->play();
}

void CSoundSource::pause()
{
	m_pControl->pause();
}

void CSoundSource::stop()
{
	m_pControl->stop();
}

void CSoundSource::rewind()
{
	m_pControl->rewind();
}

void CSoundSource::update(float dt)
{
	CVector3f oldPos = getPosition();
	const CMatrix4f& mat = ltm();
	setPosition(mat.position());
	setDirection(mat.at());
	setVelocity((mat.position() - oldPos) / dt);
	//ISceneNode::update(dt);
}

void CSoundSource::setPosition(CVector3f pos)
{
	ALfloat vec[3] = {pos.x, pos.y, -pos.z};
	alSourcefv(m_sourceId, AL_POSITION, vec);
}

CVector3f CSoundSource::getPosition() const
{
	CVector3f temp;
	alGetSourcefv(m_sourceId, AL_POSITION, temp.ptr());
	temp.z = -temp.z;
	return temp;
}

void CSoundSource::setVelocity(CVector3f pos)
{
	ALfloat vec[3] = {pos.x, pos.y, -pos.z};
	alSourcefv(m_sourceId, AL_VELOCITY, vec);
}

CVector3f CSoundSource::getVelocity() const
{
	CVector3f temp;
	alGetSourcefv(m_sourceId, AL_VELOCITY, temp.ptr());
	temp.z = -temp.z;
	return temp;
}

void CSoundSource::setDirection(CVector3f pos)
{
	ALfloat vec[3] = {pos.x, pos.y, -pos.z};
	alSourcefv(m_sourceId, AL_DIRECTION, vec);
}

CVector3f CSoundSource::getDirection() const
{
	CVector3f temp;
	alGetSourcefv(m_sourceId, AL_DIRECTION, temp.ptr());
	temp.z = -temp.z;
	return temp;
}

void CSoundSource::setPitch(float pitch)
{
	pitch = max(0.01f, pitch);
	pitch = min(2.0f, pitch);
	alSourcef(m_sourceId, AL_PITCH, pitch);
}

float CSoundSource::getPitch() const
{
	float pitch;
	alGetSourcef(m_sourceId, AL_PITCH, &pitch);
	return pitch;
}

void CSoundSource::setGain(float arg)
{
	alSourcef(m_sourceId, AL_GAIN, arg);
}

float CSoundSource::getGain() const
{
	float temp;
	alGetSourcef(m_sourceId, AL_GAIN, &temp);
	return temp;
}

void CSoundSource::setMinGain(float arg)
{
	alSourcef(m_sourceId, AL_MIN_GAIN, arg);
}

float CSoundSource::getMinGain() const
{
	float temp;
	alGetSourcef(m_sourceId, AL_MIN_GAIN, &temp);
	return temp;
}

void CSoundSource::setMaxGain(float arg)
{
	alSourcef(m_sourceId, AL_MAX_GAIN, arg);
}

float CSoundSource::getMaxGain() const
{
	float temp;
	alGetSourcef(m_sourceId, AL_MAX_GAIN, &temp);
	return temp;
}

void CSoundSource::setMaxDistance(float arg)
{
	alSourcef(m_sourceId, AL_MAX_DISTANCE, arg);
}

float CSoundSource::getMaxDistance() const
{
	float temp;
	alGetSourcef(m_sourceId, AL_MAX_DISTANCE, &temp);
	return temp;
}

void CSoundSource::setRolloffFactor(float arg)
{
	alSourcef(m_sourceId, AL_ROLLOFF_FACTOR, arg);
}

float CSoundSource::getRolloffFactor() const
{
	float temp;
	alGetSourcef(m_sourceId, AL_ROLLOFF_FACTOR, &temp);
	return temp;
}

void CSoundSource::setReferenceDistance(float arg)
{
	alSourcef(m_sourceId, AL_REFERENCE_DISTANCE, arg);
}

float CSoundSource::getReferenceDistance() const
{
	float temp;
	alGetSourcef(m_sourceId, AL_REFERENCE_DISTANCE, &temp);
	return temp;
}

void CSoundSource::setConeOuterGain(float arg)
{
	alSourcef(m_sourceId, AL_CONE_OUTER_GAIN, arg);
}

float CSoundSource::getConeOuterGain() const
{
	float temp;
	alGetSourcef(m_sourceId, AL_CONE_OUTER_GAIN, &temp);
	return temp;
}

void CSoundSource::setConeInnerAngle(float arg)
{
	alSourcef(m_sourceId, AL_CONE_INNER_ANGLE, arg);
}

float CSoundSource::getConeInnerAngle() const
{
	float temp;
	alGetSourcef(m_sourceId, AL_CONE_INNER_ANGLE, &temp);
	return temp;
}

void CSoundSource::setConeOuterAngle(float arg)
{
	alSourcef(m_sourceId, AL_CONE_OUTER_ANGLE, arg);
}

float CSoundSource::getConeOuterAngle() const
{
	float temp;
	alGetSourcef(m_sourceId, AL_CONE_OUTER_ANGLE, &temp);
	return temp;
}

void CSoundSource::setRelativeToListener(bool arg)
{
	alSourcei(m_sourceId, AL_SOURCE_RELATIVE, (arg?AL_TRUE:AL_FALSE));
}

bool CSoundSource::getRelativeToListener() const
{
	int temp;
	alGetSourcei(m_sourceId, AL_SOURCE_RELATIVE, &temp);
	return temp==AL_TRUE;
}

void CSoundSource::setLooping(bool arg)
{
	return m_pControl->setLooping(arg);
}

bool CSoundSource::getLooping() const
{
	return m_pControl->getLooping();
}

void CSoundSource::setSound(ISound* pSound)
{
	if(m_pSound != pSound)
	{
		if(m_pSound)
		{
			stop();
			m_pSound->release();

			// Remove from Sound-list
			vector<ISoundSourceControl*>::iterator it = find(Sound::ms_streamList.begin(), Sound::ms_streamList.end(), m_pControl);
			if(it != Sound::ms_streamList.end())
				Sound::ms_streamList.erase(it);

			safeDelete(m_pControl);
		}
		m_pSound = pSound;
		if(m_pSound)
		{
			m_pSound->addRef();
			m_pControl = m_pSound->getNewControl(this);
			if(dynamic_cast<ISoundStream*>(m_pSound))
				Sound::ms_streamList.push_back(m_pControl);
		}
	}
}

ISound* CSoundSource::getSound() 
{
	return m_pSound;
}

void CSoundSource::setState(State arg)
{
	int state;
	switch (arg)
	{
	case INITIAL:	state=AL_INITIAL; break;
	case PLAYING:	state=AL_PLAYING; break;
	case PAUSED:	state=AL_PAUSED; break;
	case STOPPED:	state=AL_STOPPED; break;
	default:		throw domain_error("Argument must be one of INITIAL, PLAYING, PAUSED or STOPPED");
	};
	alSourcei(m_sourceId, AL_SOURCE_STATE, state);
}

CSoundSource::State CSoundSource::getState() const
{
	int temp;
	alGetSourcei(m_sourceId, AL_SOURCE_STATE, &temp);
	switch (temp)
	{
	case AL_INITIAL:	return INITIAL;
	case AL_PLAYING:	return PLAYING;
	case AL_PAUSED:		return PAUSED;
	default:			return STOPPED;
	};
}

void CSoundSource::setDirectFilter(const IFilter& filter)
{
	alSourcei(m_sourceId, AL_DIRECT_FILTER, filter.m_filterId);
}

void CSoundSource::disableDirectFilter()
{
	alSourcei(m_sourceId, AL_DIRECT_FILTER, AL_FILTER_NULL);
}

void CSoundSource::setAuxSend(int n, const CEffectGroup& eg)
{
	alSource3i(m_sourceId, AL_AUXILIARY_SEND_FILTER, eg.m_effectSlot, n, AL_FILTER_NULL);
}

void CSoundSource::setAuxSend(int n, const CEffectGroup& eg, const IFilter& filter)
{
	alSource3i(m_sourceId, AL_AUXILIARY_SEND_FILTER, eg.m_effectSlot, n, filter.m_filterId);
}

void CSoundSource::disableAuxSend(int n)
{
	alSource3i(m_sourceId, AL_AUXILIARY_SEND_FILTER, AL_EFFECTSLOT_NULL, n, AL_FILTER_NULL);
}

//////////////////////////////////////////////////////////////////////////

void CSoundListener::update(float dt)
{
	CVector3f oldPos = Sound::getListenerPosition();
	const CMatrix4f& mat = ltm();
	Sound::setListener(mat);
	Sound::setListenerVelocity((mat.position() - oldPos) / dt);
	//ISceneNode::update(dt);
}
