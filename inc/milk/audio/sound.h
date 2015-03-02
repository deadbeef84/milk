#ifndef MILK_SOUND_H_
#define MILK_SOUND_H_

#include "milk/math/cvector.h"
#include "milk/math/cmatrix.h"
#include "milk/includes.h"
#include "milk/boost.h"
#include "milk/iresource.h"
#include "milk/scenegraph/iscenenode.h"
#include <string>
#include <utility>
#include <vector>

// OpenAL includes
#include <al.h>
#include <alc.h>

// Ogg-vorbis include
#include <vorbis/vorbisfile.h>

// SDL
#include <SDL.h>

namespace milk
{
	// forward declaration
	class CSoundSource;
	class ISoundSourceControl;

	// controls general sound-stuff
	class Sound
	{
		friend class CSoundSource;
	public:
		static void init();
		static void free();
		static void update();

		static void setListener(const CMatrix4f& mat);
		static CMatrix4f getListener();

		static void setListenerPosition(CVector3f pos);
		static CVector3f getListenerPosition();

		static void setListenerVelocity(CVector3f vel);
		static CVector3f getListenerVelocity();

		static void setListenerOrientation(const CVector3f& at, const CVector3f& up);
		static void setListenerOrientation(const CMatrix3f& mat);
		static void setListenerOrientation(const CMatrix4f& mat);
		static CMatrix3f getListenerOrientation();

		/// sets the listener gain. this is in fact like setting the global value
		static void setListenerGain(float arg);
		/// gets the listener gain.
		static float getListenerGain();

		enum DistanceModel
		{
			NONE,
			INVERSE_DISTANCE,
			INVERSE_DISTANCE_CLAMPED,
			LINEAR_DISTANCE,
			LINEAR_DISTANCE_CLAMPED,
			EXPONENT_DISTANCE,
			EXPONENT_DISTANCE_CLAMPED
		};

		/// set speed of sound. 344 by default (speed of sound in air in meters/second)
		static void setSpeedOfSound(float vel);
		/// get speed of sound. 344 by default (speed of sound in air in meters/second)
		static float getSpeedOfSound();

		///default is 1, used to (de)exaggerate the effect of the doppler effect
		static void setDopplerFactor(float factor);	
		///default is 1, used to (de)exaggerate the effect of the doppler effect
		static float getDopplerFactor();

		///default is INVERSE_DISTANCE
		static void setDistanceModel(DistanceModel model);
		///default is INVERSE_DISTANCE
		static DistanceModel getDistanceModel();

		///throws an exception on errors
		static void checkForErrors();


		static std::string vendor()
		{ return (char*)alGetString(AL_VENDOR); }
		static std::string version()
		{ return (char*)alGetString(AL_VERSION); }
		static std::string renderer()
		{ return (char*)alGetString(AL_RENDERER); }
		static std::string extensions()
		{ return (char*)alGetString(AL_EXTENSIONS); }

		static ALenum sdlToAlFormat(const SDL_AudioSpec& audiospec)
		{
			if(audiospec.format == AUDIO_U8 || audiospec.format == AUDIO_S8)
				return (audiospec.channels == 1) ? AL_FORMAT_MONO8 : AL_FORMAT_STEREO8;
			else
				return (audiospec.channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
		}

	private:
		Sound();

		static ALCdevice* ms_pDevice;
		static ALCcontext* ms_pContext;

		static std::vector<ISoundSourceControl*> ms_streamList;
	};
}

#endif
