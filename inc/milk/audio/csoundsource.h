#ifndef MILK_CSOUNDSOURCE_H_
#define MILK_CSOUNDSOURCE_H_

#include "milk/math/cvector.h"
#include "milk/math/cmatrix.h"
#include "milk/includes.h"
#include "milk/scenegraph/iscenenode.h"
#include <string>
#include <utility>
#include <vector>

// OpenAL includes
#include <al.h>
//#include <alut.h>
#include <alc.h>

// Ogg-vorbis include
#include <vorbis/vorbisfile.h>

namespace milk
{
	// forward declaration
	class ISound;
	class ISoundSourceControl;
	class IFilter;
	class CEffectGroup;

	/// sound-emitter
	class CSoundSource : public ISceneNode
	{
		friend class Sound;
		friend class ISoundSourceControl;
	public:
		CSoundSource();
		CSoundSource(ISound* pSound);
		virtual ~CSoundSource();

		virtual void update(float dt);

		void play();
		void pause();
		void stop();
		void rewind();

		void setPosition(CVector3f);
		CVector3f getPosition() const;

		void setVelocity(CVector3f);
		CVector3f getVelocity() const;

		void setDirection(CVector3f);
		CVector3f getDirection() const;

		// set/get pitch (clamped to [0,2]), does affect speed
		void setPitch(float pitch);
		float getPitch() const;

		// set/get volume
		void setGain(float arg);
		float getGain() const;

		void setMaxDistance(float arg);
		float getMaxDistance() const;

		void setRolloffFactor(float arg);
		float getRolloffFactor() const;

		void setReferenceDistance(float arg);
		float getReferenceDistance() const;

		void setMinGain(float arg);
		float getMinGain() const;

		void setMaxGain(float arg);
		float getMaxGain() const;

		void setConeOuterGain(float arg);
		float getConeOuterGain() const;

		void setConeInnerAngle(float arg);
		float getConeInnerAngle() const;

		void setConeOuterAngle(float arg);
		float getConeOuterAngle() const;

		/// is the position relative to the listener? false by default
		void setRelativeToListener(bool arg);
		bool getRelativeToListener() const;

		void setLooping(bool);
		bool getLooping() const;

		void setSound(ISound* sound);
		ISound* getSound();

		enum State
		{
			INITIAL,
			PLAYING,
			PAUSED,
			STOPPED
		};

		void setState(State arg);
		State getState() const;

		//////////////////////////////

		void setDirectFilter(const IFilter& filter);
		void disableDirectFilter();

		void setAuxSend(int n, const CEffectGroup& eg);
		void setAuxSend(int n, const CEffectGroup& eg, const IFilter& filter);
		void disableAuxSend(int n);

	protected:
		ALuint m_sourceId;
		ISound *m_pSound;
		ISoundSourceControl *m_pControl;

		CSoundSource(CSoundSource&);
		CSoundSource& operator=(CSoundSource&);
	};


	class CSoundListener : public ISceneNode
	{
	public:
		virtual ~CSoundListener()
		{ }

		virtual void update(float dt);
	};
}

#endif
