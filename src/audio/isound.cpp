#include "milk/audio/isound.h"
#include "milk/audio/csoundsource.h"
#include "milk/error.h"
#include "milk/helper.h"
#include "milk/vectorPOD.h"
#include "milk/boost.h"
#include "milk/audio/sound.h"
using namespace milk;
using namespace std;

// ogg callbacks
size_t ogg_read(void *ptr, size_t size, size_t nmemb, void *datasource)
{ return fread(ptr, size, nmemb, (FILE*)datasource); }
int ogg_seek(void *datasource, ogg_int64_t offset, int whence)
{ return fseek((FILE*)datasource, (long)offset, whence); }
int ogg_close(void *datasource)
{ return fclose((FILE*)datasource); }
long ogg_tell(void *datasource)
{ return ftell((FILE*)datasource); }

/*
static void loadOggVorbis(std::string filename, std::vector<char>& buffer, ALenum* format, ALsizei* freq)
{
	int endian = 0; // 0 for Little-Endian, 1 for Big-Endian
	int bitStream;
	int bytes;
	const int OGG_BUFFER_READ_SIZE = 32768;
	char array[OGG_BUFFER_READ_SIZE];
	FILE *f;

	// Open for binary reading
	f = fopen(filename.c_str(), "rb");

	if(!f)
		throw error::file("loadOggVorbis: Cannot open "+filename+" for reading...");

	vorbis_info *pInfo;
	OggVorbis_File oggFile;

	// Try opening the given file
	if(ov_open(f, &oggFile, NULL, 0))
		throw error::sound("loadOggVorbis: Error opening "+filename+" for decoding...");

	// Get some information about the OGG file
	pInfo = ov_info(&oggFile, -1);

	// Check the number of channels... always use 16-bit samples
	*format = (pInfo->channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

	// The frequency of the sampling rate
	*freq = pInfo->rate;

	// Keep reading until all is read
	do
	{
		// Read up to a buffer's worth of decoded sound data
		bytes = ov_read(&oggFile, array, OGG_BUFFER_READ_SIZE, endian, 2, 1, &bitStream);

		if(bytes < 0)
		{
			ov_clear(&oggFile);
			throw error::sound("loadOggVorbis: Error decoding "+filename+"...");
		}

		// Append to end of buffer
		buffer.insert(buffer.end(), array, array + bytes);
	}
	while (bytes > 0);

	// Clean up!
	ov_clear(&oggFile);
}
*/

ALuint ISoundSourceControl::getSourceId() const
{
	return m_pSoundSource->m_sourceId;
}

////////////////////////////////////////////////

ISound* ISound::create(const string& rid)
{
	if(!fileExists(rid))
		throw error::file("Could not find file \""+rid+"\".");

	if(text::endsWith(rid, ".ogg"))
		return new CSoundStream_Ogg(rid);
	else
		return new CSound_Wave(rid);
}

CSound_Wave::CSound_Wave(const string& file)
{
	alGenBuffers(1, &m_bufferId);

	// FIXME!!!
	//ALenum format;
	//ALvoid *data;
	//ALsizei size,freq;
	//ALboolean loop;
	//alutLoadWAVFile(reinterpret_cast<ALbyte*>(const_cast<char*>(file.c_str())), &format, &data, &size, &freq, &loop);
	//alBufferData(m_bufferId, format, data, size, freq);
	//alutUnloadWAV(format, data, size, freq);

	SDL_AudioSpec wavSpec;
	Uint32 wavLength;
	Uint8 *wavBuffer;

	// Load the WAV
	if(!SDL_LoadWAV(file.c_str(), &wavSpec, &wavBuffer, &wavLength))
		throw error::sound("Could not open "+file+": " + SDL_GetError());

	alBufferData(m_bufferId, Sound::sdlToAlFormat(wavSpec), wavBuffer, wavLength, wavSpec.freq);

	// Free It
	SDL_FreeWAV(wavBuffer);
}

CSound_Wave::~CSound_Wave()
{
	alDeleteBuffers(1, &m_bufferId);
}

int CSound_Wave::getFreqency() const
{
	int temp;
	alGetBufferi(m_bufferId, AL_FREQUENCY, &temp);
	return temp;
}

int CSound_Wave::getBits() const
{
	int temp;
	alGetBufferi(m_bufferId, AL_BITS, &temp);
	return temp;
}

int CSound_Wave::getChannels() const
{
	int temp;
	alGetBufferi(m_bufferId, AL_CHANNELS, &temp);
	return temp;
}

int CSound_Wave::getSize() const
{
	int temp;
	alGetBufferi(m_bufferId, AL_SIZE, &temp);
	return temp;
}

CSoundSourceControl_Wave::CSoundSourceControl_Wave(CSoundSource *pSoundSource, CSound_Wave* pSound)
: ISoundSourceControl(pSoundSource)
{
	alSourcei(getSourceId(), AL_BUFFER, pSound->m_bufferId);
}

void CSoundSourceControl_Wave::play()
{
	alSourcePlay(getSourceId());
}

void CSoundSourceControl_Wave::pause()
{
	alSourcePause(getSourceId());
}

void CSoundSourceControl_Wave::stop()
{
	alSourceStop(getSourceId());
}

void CSoundSourceControl_Wave::rewind()
{
	alSourceRewind(getSourceId());
}

void CSoundSourceControl_Wave::setLooping(bool loop)
{
	alSourcei(getSourceId(), AL_LOOPING, (loop?AL_TRUE:AL_FALSE));
}

bool CSoundSourceControl_Wave::getLooping() const
{
	int temp;
	alGetSourcei(getSourceId(), AL_LOOPING, &temp);
	return temp==AL_TRUE;
}

//////////////////////////////////////////////////////////////////////////

CSoundSourceControl_Ogg::CSoundSourceControl_Ogg(CSoundSource *pSoundSource, CSoundStream_Ogg* pSound)
: ISoundSourceControl(pSoundSource)
{
	// Open for binary reading
	FILE *f = fopen(pSound->m_filename.c_str(), "rb");

	if(!f)
		throw error::sound("CSoundStream_Ogg::load() Cannot open "+pSound->m_filename+" for reading...");

	ov_callbacks cbs = {ogg_read, ogg_seek, ogg_close, ogg_tell};

	// Try opening the given file
	if(ov_open_callbacks(f, &m_oggFile, 0, 0, cbs))
		throw error::sound("CSoundStream_Ogg::load() Error opening "+pSound->m_filename+" for decoding...");

	// Get some information about the OGG file
	vorbis_info *pInfo = ov_info(&m_oggFile, -1);
	m_format = (pInfo->channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
	m_freq = pInfo->rate;
	m_loaded = true;

	// We don't want the buffers to loop
	alSourcei(getSourceId(), AL_LOOPING, AL_FALSE);

	// Generate buffers
	alGenBuffers(BUFFER_COUNT, m_bufferId);

	// Load and queue buffers
	for(int i = 0; i < BUFFER_COUNT; i++)
	{
		if(loadBuffer(m_bufferId[i]))
			alSourceQueueBuffers(getSourceId(), 1, &m_bufferId[i]);
		else
			break;
	}
}

CSoundSourceControl_Ogg::~CSoundSourceControl_Ogg()
{
	ALint queued;
	alGetSourcei(getSourceId(), AL_BUFFERS_QUEUED, &queued);
	while(queued--)
	{
		ALuint buffer;
		alSourceUnqueueBuffers(getSourceId(), 1, &buffer);
	}

	// Delete buffers
	alDeleteBuffers(BUFFER_COUNT, m_bufferId);

	if(m_loaded)
	{
		// Unload ogg-vorbis-file
		ov_clear(&m_oggFile);
		m_loaded = false;
	}
}

void CSoundSourceControl_Ogg::play()
{
	m_playing = true;
	alSourcePlay(getSourceId());
}

void CSoundSourceControl_Ogg::pause()
{
	m_playing = false;
	alSourcePause(getSourceId());
}

void CSoundSourceControl_Ogg::stop()
{
	m_playing = false;
	alSourceStop(getSourceId());
}

void CSoundSourceControl_Ogg::rewind()
{
	// Unqueue buffers
	ALint queued;
	alGetSourcei(getSourceId(), AL_BUFFERS_QUEUED, &queued);
	while(queued--)
	{
		ALuint buffer;
		alSourceUnqueueBuffers(getSourceId(), 1, &buffer);
	}

	// Seek to beginning
	ov_raw_seek(&m_oggFile, 0);

	// Load and queue buffers
	for(int i = 0; i < BUFFER_COUNT; i++)
	{
		if(loadBuffer(m_bufferId[i]))
			alSourceQueueBuffers(getSourceId(), 1, &m_bufferId[i]);
		else
			break;
	}
}

void CSoundSourceControl_Ogg::setLooping(bool loop)
{
	m_looping = loop;
}

bool CSoundSourceControl_Ogg::getLooping() const
{
	return m_looping;
}

bool CSoundSourceControl_Ogg::loadBuffer(ALuint bufferId)
{
	int endian = 0; // 0 for Little-Endian, 1 for Big-Endian
	int bitStream, bytes;
	ALsizei size = 0;
	const size_t OGG_BUFFER_SIZE = 32768;
	char array[OGG_BUFFER_SIZE];

	// Keep reading until all is read
	while(size < OGG_BUFFER_SIZE)
	{
		// Read up to a buffer's worth of decoded sound data
		bytes = ov_read(&m_oggFile, array+size, OGG_BUFFER_SIZE-size, endian, 2, 1, &bitStream);
		if(bytes < 0)
		{
			ov_clear(&m_oggFile);
			throw error::sound("loadOggVorbis: Error while decoding...");
		}
		else if(bytes > 0)
			size += bytes;
		else if(m_looping)
			ov_raw_seek(&m_oggFile, 0);
		else
		{
			m_playing = false;
			break;
		}
	}

	if(!size)
		return false;

	alBufferData(bufferId, m_format, array, size, m_freq);
	return true;
}

void CSoundSourceControl_Ogg::enqueueBuffers()
{
	if(m_pSoundSource->getState() == CSoundSource::PLAYING)
	{
		ALuint buffer;
		int processed = 0;
		alGetSourcei(getSourceId(), AL_BUFFERS_PROCESSED, &processed); /* FIXME: why doesn't any buffers get processed if we have a buffer underrun? */
		if(processed)
		{
			while(processed--)
			{
				alSourceUnqueueBuffers(getSourceId(), 1, &buffer);
				if(loadBuffer(buffer))
					alSourceQueueBuffers(getSourceId(), 1, &buffer);
			}
		}
	}
	else if(m_pSoundSource->getState() == CSoundSource::STOPPED && m_playing)
	{
		// Unqueue buffers
		ALint queued;
		alGetSourcei(getSourceId(), AL_BUFFERS_QUEUED, &queued);
		while(queued--)
		{
			ALuint buffer;
			alSourceUnqueueBuffers(getSourceId(), 1, &buffer);
		}

		// Load and queue buffers
		for(int i = 0; i < BUFFER_COUNT; i++)
		{
			if(loadBuffer(m_bufferId[i]))
				alSourceQueueBuffers(getSourceId(), 1, &m_bufferId[i]);
			else
				break;
		}
		alSourcePlay(getSourceId());
	}
}

/////////////////////////////////////////////////////////////

CSoundStream_Ogg::CSoundStream_Ogg(const std::string& filename)
{
	load(filename);
}

CSoundStream_Ogg::~CSoundStream_Ogg()
{
	unload();
}

void CSoundStream_Ogg::load(const std::string& filename)
{
	unload();
	m_filename = filename;
}

void CSoundStream_Ogg::unload()
{
	m_filename = "";
}

int CSoundStream_Ogg::getFreqency() const
{
	return 0; // TODO
}

int CSoundStream_Ogg::getBits() const
{
	return 0; // TODO
}

int CSoundStream_Ogg::getChannels() const
{
	return 0; // TODO
}

int CSoundStream_Ogg::getSize() const
{
	return 0; // TODO
}
