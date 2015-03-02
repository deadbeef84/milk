#include "milk/audio/sound.h"
#include "milk/audio/alew.h"
#include "milk/audio/isound.h"
#include "milk/error.h"
#include "milk/helper.h"
#include "milk/vectorPOD.h"
#include "milk/boost.h"
#include "milk/dlghelper.h"
using namespace milk;
using namespace std;

vector<ISoundSourceControl*> Sound::ms_streamList;
ALCdevice* Sound::ms_pDevice = 0;
ALCcontext* Sound::ms_pContext = 0;

void Sound::setListener(const CMatrix4f& mat)
{
	setListenerOrientation(mat);
	setListenerPosition(mat.position());
}

CMatrix4f Sound::getListener()
{
	CMatrix4f mat;
	matrixSetRotation(mat, getListenerOrientation());
	mat.position() = getListenerPosition();
	return mat;
}

void Sound::setListenerPosition(CVector3f pos)
{
	ALfloat vec[3] = {pos.x, pos.y, -pos.z};
	alListenerfv(AL_POSITION, vec);
}

CVector3f Sound::getListenerPosition()
{
	CVector3f temp;
	alGetListenerfv(AL_POSITION, temp.ptr());
	temp.z = -temp.z;
	return temp;
}

void Sound::setListenerVelocity(CVector3f pos)
{
	ALfloat vec[3] = {pos.x, pos.y, -pos.z};
	alListenerfv(AL_VELOCITY, vec);
}

CVector3f Sound::getListenerVelocity()
{
	CVector3f temp;
	alGetListenerfv(AL_VELOCITY, temp.ptr());
	temp.z = -temp.z;
	return temp;
}

void Sound::setListenerOrientation(const CVector3f& at, const CVector3f& up)
{
	ALfloat vec[6] = { at.x, at.y,-at.z,
				       up.x, up.y,-up.z };
	alListenerfv(AL_ORIENTATION, vec);
}

void Sound::setListenerOrientation(const CMatrix4f& mat)
{
	setListenerOrientation(mat.at(), mat.up());
}

void Sound::setListenerOrientation(const CMatrix3f& mat)
{
	setListenerOrientation(mat.at(), mat.up());
}

CMatrix3f Sound::getListenerOrientation()
{
	float temp[6];
	alGetListenerfv(AL_ORIENTATION, temp);
	temp[2] = -temp[2];
	temp[5] = -temp[5];
	CVector3f at(temp);
	CVector3f up(temp+3);
	return CMatrix3f(cross(at,up), up, at);
}

void Sound::setListenerGain(float arg)
{
	alListenerf(AL_GAIN, arg);
}

float Sound::getListenerGain()
{
	float temp;
	alGetListenerf(AL_GAIN, &temp);
	return temp;
}

void Sound::setSpeedOfSound(float vel)
{
	alSpeedOfSound(vel);
}

float Sound::getSpeedOfSound()
{
	return alGetFloat(AL_SPEED_OF_SOUND);
}

void Sound::setDopplerFactor(float factor)
{
	alDopplerFactor(factor);
}

float Sound::getDopplerFactor()
{
	return alGetFloat(AL_DOPPLER_FACTOR);
}

void Sound::setDistanceModel(DistanceModel model)
{
	switch (model)
	{
	case NONE:						alDistanceModel(AL_NONE); break;
	case INVERSE_DISTANCE:			alDistanceModel(AL_INVERSE_DISTANCE); break;
	case INVERSE_DISTANCE_CLAMPED:	alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED); break;
	case LINEAR_DISTANCE:			alDistanceModel(AL_LINEAR_DISTANCE); break;
	case LINEAR_DISTANCE_CLAMPED:	alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED); break;
	case EXPONENT_DISTANCE:			alDistanceModel(AL_EXPONENT_DISTANCE); break;
	case EXPONENT_DISTANCE_CLAMPED:	alDistanceModel(AL_EXPONENT_DISTANCE_CLAMPED); break;
	default: throw error::sound("Invalid distance model.");
	}
}

Sound::DistanceModel Sound::getDistanceModel()
{
	switch (alGetInteger(AL_DISTANCE_MODEL))
	{
	case AL_NONE:				return NONE;
	case AL_INVERSE_DISTANCE:	return INVERSE_DISTANCE;
	default:					return INVERSE_DISTANCE_CLAMPED;
	}
}

void Sound::checkForErrors()
{
	int error = alGetError();
	if(error != AL_NO_ERROR)
	{
		string str = "OpenAL-error " + toStr(error) + ": \"";
		switch (error)
		{
		case AL_INVALID_NAME:		str += "AL_INVALID_NAME";		break;
		case AL_INVALID_VALUE:		str += "AL_INVALID_VALUE";		break;
		case AL_OUT_OF_MEMORY:		str += "AL_OUT_OF_MEMORY";		break;
#ifdef AL_INVALID_ENUM
		case AL_INVALID_ENUM:		str += "AL_INVALID_ENUM";		break;
#endif
#ifdef AL_INVALID_OPERATION
		case AL_INVALID_OPERATION:	str += "AL_INVALID_OPERATION";	break;
#endif
		default:					str += "of unknown type";		break;
		}
		throw error::sound(str+"\"");
	}
}

void Sound::init()
{
	//const ALCchar *pDevices = alcGetString(0, ALC_DEVICE_SPECIFIER);
	//const ALCchar *pDefaultDevice = alcGetString(0, ALC_DEFAULT_DEVICE_SPECIFIER);
	ALCchar deviceName[] = "Generic Software";
	ms_pDevice = alcOpenDevice(deviceName);
	//ms_pDevice = alcOpenDevice(pDefaultDevice);
	//ms_pDevice = alcOpenDevice(NULL);

	const ALCchar *pOpenDevice = alcGetString(ms_pDevice, ALC_DEVICE_SPECIFIER);

	ALint attribs[4] = {ALC_MAX_AUXILIARY_SENDS, 4, 0, 0};
	ms_pContext = alcCreateContext(ms_pDevice, attribs);
	alcMakeContextCurrent(ms_pContext);

	alewInit(ms_pDevice);

	ALCint sends;
	alcGetIntegerv(ms_pDevice, ALC_MAX_AUXILIARY_SENDS, 1, &sends);
	/*
	dialog::alert("OpenAL", (string)"Device: "+pOpenDevice+
							"\nEFX: " + (ALEW_EXT_efx?"Yes":"No")+
							"\nAuxiliary sends: "+toStr(sends));
	*/
}

void Sound::free()
{
	alcMakeContextCurrent(NULL);
	alcDestroyContext(ms_pContext);
	alcCloseDevice(ms_pDevice);	
	ms_pDevice = 0;
	ms_pContext = 0;
}

void Sound::update()
{
	alcProcessContext(ms_pContext);
	for(vector<ISoundSourceControl*>::iterator it = ms_streamList.begin(); it != ms_streamList.end(); ++it)
		(*it)->enqueueBuffers();
}
