#ifndef MILK_ISOUND_H_
#define MILK_ISOUND_H_

#include "milk/math/cvector.h"
#include "milk/math/cmatrix.h"
#include "milk/includes.h"
#include "milk/iresource.h"
#include "milk/scenegraph/iscenenode.h"
#include <string>
#include <utility>

// OpenAL includes
#include <al.h>
//#include <alut.h>
#include <alc.h>

// Ogg-vorbis include
#include <vorbis/vorbisfile.h>

namespace milk
{
	// forward declaration
	class CSoundSource;
	class CSound_Wave;
	class CSoundStream_Ogg;

	// Used to control the playback of a sound source. Internal use only.
	class ISoundSourceControl
	{
		friend class Sound;
	public:
		ISoundSourceControl(CSoundSource *pSoundSource)
			: m_pSoundSource(pSoundSource)
		{ }
		virtual ~ISoundSourceControl()
		{ }

		virtual void play()=0;
		virtual void pause()=0;
		virtual void stop()=0;
		virtual void rewind()=0;
		virtual void setLooping(bool loop)=0;
		virtual bool getLooping() const =0;

		// only for streaming sounds
		virtual void enqueueBuffers()
		{ }

	protected:
		ALuint getSourceId() const;

		CSoundSource *m_pSoundSource;
	};

	/// for loading and containing sound.
	class ISound : public IResource
	{
		friend class CSoundSource;
	public:
		typedef ISound ResourceType;

		virtual ~ISound() { }

		MILK_MAKE_RESOURCE(ISound);

		static ISound* create(const std::string& rid);

		virtual int getFreqency() const =0;
		virtual int getBits() const =0;
		virtual int getChannels() const =0;
		virtual int getSize() const =0;

	protected:
		virtual ISoundSourceControl* getNewControl(CSoundSource *pSoundSource)=0;
	};

	/// for loading and containing streaming sound.
	class ISoundStream : public ISound
	{
	public:
		ISoundStream() { }
		virtual ~ISoundStream() { }
	};

	class CSoundSourceControl_Wave : public ISoundSourceControl
	{
	public:
		CSoundSourceControl_Wave(CSoundSource *pSoundSource, CSound_Wave* pSound);
		virtual ~CSoundSourceControl_Wave()
		{ }

		virtual void play();
		virtual void pause();
		virtual void stop();
		virtual void rewind();
		virtual void setLooping(bool loop);
		virtual bool getLooping() const;
	};

	/// for loading and containing sound.
	class CSound_Wave : public ISound
	{
		friend class CSoundSourceControl_Wave;
	public:
		CSound_Wave(const std::string& file);
		virtual ~CSound_Wave();

		virtual int getFreqency() const;
		virtual int getBits() const;
		virtual int getChannels() const;
		virtual int getSize() const;

	protected:
		virtual ISoundSourceControl* getNewControl(CSoundSource* pSoundSource)
		{ return new CSoundSourceControl_Wave(pSoundSource, this); }

		ALuint m_bufferId;
	};

	class CSoundSourceControl_Ogg : public ISoundSourceControl
	{
	public:
		CSoundSourceControl_Ogg(CSoundSource *pSoundSource, CSoundStream_Ogg* pSound);
		virtual ~CSoundSourceControl_Ogg();

		virtual void play();
		virtual void pause();
		virtual void stop();
		virtual void rewind();
		virtual void setLooping(bool loop);
		virtual bool getLooping() const;

		virtual void enqueueBuffers();

	private:
		bool loadBuffer(ALuint bufferId);

		enum { BUFFER_COUNT = 10 };
		ALuint m_bufferId[BUFFER_COUNT];
		OggVorbis_File m_oggFile;
		ogg_int64_t m_streamPos;
		bool m_loaded;
		ALenum m_format;
		ALsizei m_freq;
		bool m_playing;
		bool m_looping;
	};

	class CSoundStream_Ogg : public ISoundStream
	{
		friend class CSoundSourceControl_Ogg;
	public:
		CSoundStream_Ogg(const std::string& filename);
		virtual ~CSoundStream_Ogg();

		void load(const std::string& filename);
		void unload();

		virtual int getFreqency() const;
		virtual int getBits() const;
		virtual int getChannels() const;
		virtual int getSize() const;

	protected:
		virtual ISoundSourceControl* getNewControl(CSoundSource* pSoundSource)
		{ return new CSoundSourceControl_Ogg(pSoundSource, this); }

		std::string m_filename;
	};
}

#endif
