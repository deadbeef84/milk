#include "milk/audio/alew.h"

// Effect objects
LPALGENEFFECTS alGenEffects = 0;
LPALDELETEEFFECTS alDeleteEffects = 0;
LPALISEFFECT alIsEffect = 0;
LPALEFFECTI alEffecti = 0;
LPALEFFECTIV alEffectiv = 0;
LPALEFFECTF alEffectf = 0;
LPALEFFECTFV alEffectfv = 0;
LPALGETEFFECTI alGetEffecti = 0;
LPALGETEFFECTIV alGetEffectiv = 0;
LPALGETEFFECTF alGetEffectf = 0;
LPALGETEFFECTFV alGetEffectfv = 0;

//Filter objects
LPALGENFILTERS alGenFilters = 0;
LPALDELETEFILTERS alDeleteFilters = 0;
LPALISFILTER alIsFilter = 0;
LPALFILTERI alFilteri = 0;
LPALFILTERIV alFilteriv = 0;
LPALFILTERF alFilterf = 0;
LPALFILTERFV alFilterfv = 0;
LPALGETFILTERI alGetFilteri = 0;
LPALGETFILTERIV alGetFilteriv = 0;
LPALGETFILTERF alGetFilterf = 0;
LPALGETFILTERFV alGetFilterfv = 0;

// Auxiliary slot object
LPALGENAUXILIARYEFFECTSLOTS alGenAuxiliaryEffectSlots = 0;
LPALDELETEAUXILIARYEFFECTSLOTS alDeleteAuxiliaryEffectSlots = 0;
LPALISAUXILIARYEFFECTSLOT alIsAuxiliaryEffectSlot = 0;
LPALAUXILIARYEFFECTSLOTI alAuxiliaryEffectSloti = 0;
LPALAUXILIARYEFFECTSLOTIV alAuxiliaryEffectSlotiv = 0;
LPALAUXILIARYEFFECTSLOTF alAuxiliaryEffectSlotf = 0;
LPALAUXILIARYEFFECTSLOTFV alAuxiliaryEffectSlotfv = 0;
LPALGETAUXILIARYEFFECTSLOTI alGetAuxiliaryEffectSloti = 0;
LPALGETAUXILIARYEFFECTSLOTIV alGetAuxiliaryEffectSlotiv = 0;
LPALGETAUXILIARYEFFECTSLOTF alGetAuxiliaryEffectSlotf = 0;
LPALGETAUXILIARYEFFECTSLOTFV alGetAuxiliaryEffectSlotfv = 0;

ALboolean ALEW_EXT_efx = AL_FALSE;

static bool _alewInit_EXT_efx()
{
	bool r = false;
	r |= ((alGenEffects = (LPALGENEFFECTS)alGetProcAddress("alGenEffects")) == 0);
	r |= ((alDeleteEffects = (LPALDELETEEFFECTS )alGetProcAddress("alDeleteEffects")) == 0);
	r |= ((alIsEffect = (LPALISEFFECT )alGetProcAddress("alIsEffect")) == 0);
	r |= ((alEffecti = (LPALEFFECTI)alGetProcAddress("alEffecti")) == 0);
	r |= ((alEffectiv = (LPALEFFECTIV)alGetProcAddress("alEffectiv")) == 0);
	r |= ((alEffectf = (LPALEFFECTF)alGetProcAddress("alEffectf")) == 0);
	r |= ((alEffectfv = (LPALEFFECTFV)alGetProcAddress("alEffectfv")) == 0);
	r |= ((alGetEffecti = (LPALGETEFFECTI)alGetProcAddress("alGetEffecti")) == 0);
	r |= ((alGetEffectiv = (LPALGETEFFECTIV)alGetProcAddress("alGetEffectiv")) == 0);
	r |= ((alGetEffectf = (LPALGETEFFECTF)alGetProcAddress("alGetEffectf")) == 0);
	r |= ((alGetEffectfv = (LPALGETEFFECTFV)alGetProcAddress("alGetEffectfv")) == 0);
	r |= ((alGenFilters = (LPALGENFILTERS)alGetProcAddress("alGenFilters")) == 0);
	r |= ((alDeleteFilters = (LPALDELETEFILTERS)alGetProcAddress("alDeleteFilters")) == 0);
	r |= ((alIsFilter = (LPALISFILTER)alGetProcAddress("alIsFilter")) == 0);
	r |= ((alFilteri = (LPALFILTERI)alGetProcAddress("alFilteri")) == 0);
	r |= ((alFilteriv = (LPALFILTERIV)alGetProcAddress("alFilteriv")) == 0);
	r |= ((alFilterf = (LPALFILTERF)alGetProcAddress("alFilterf")) == 0);
	r |= ((alFilterfv = (LPALFILTERFV)alGetProcAddress("alFilterfv")) == 0);
	r |= ((alGetFilteri = (LPALGETFILTERI )alGetProcAddress("alGetFilteri")) == 0);
	r |= ((alGetFilteriv= (LPALGETFILTERIV )alGetProcAddress("alGetFilteriv")) == 0);
	r |= ((alGetFilterf = (LPALGETFILTERF )alGetProcAddress("alGetFilterf")) == 0);
	r |= ((alGetFilterfv= (LPALGETFILTERFV )alGetProcAddress("alGetFilterfv")) == 0);
	r |= ((alGenAuxiliaryEffectSlots = (LPALGENAUXILIARYEFFECTSLOTS)alGetProcAddress("alGenAuxiliaryEffectSlots")) == 0);
	r |= ((alDeleteAuxiliaryEffectSlots = (LPALDELETEAUXILIARYEFFECTSLOTS)alGetProcAddress("alDeleteAuxiliaryEffectSlots")) == 0);
	r |= ((alIsAuxiliaryEffectSlot = (LPALISAUXILIARYEFFECTSLOT)alGetProcAddress("alIsAuxiliaryEffectSlot")) == 0);
	r |= ((alAuxiliaryEffectSloti = (LPALAUXILIARYEFFECTSLOTI)alGetProcAddress("alAuxiliaryEffectSloti")) == 0);
	r |= ((alAuxiliaryEffectSlotiv = (LPALAUXILIARYEFFECTSLOTIV)alGetProcAddress("alAuxiliaryEffectSlotiv")) == 0);
	r |= ((alAuxiliaryEffectSlotf = (LPALAUXILIARYEFFECTSLOTF)alGetProcAddress("alAuxiliaryEffectSlotf")) == 0);
	r |= ((alAuxiliaryEffectSlotfv = (LPALAUXILIARYEFFECTSLOTFV)alGetProcAddress("alAuxiliaryEffectSlotfv")) == 0);
	r |= ((alGetAuxiliaryEffectSloti = (LPALGETAUXILIARYEFFECTSLOTI)alGetProcAddress("alGetAuxiliaryEffectSloti")) == 0);
	r |= ((alGetAuxiliaryEffectSlotiv = (LPALGETAUXILIARYEFFECTSLOTIV)alGetProcAddress("alGetAuxiliaryEffectSlotiv")) == 0);
	r |= ((alGetAuxiliaryEffectSlotf = (LPALGETAUXILIARYEFFECTSLOTF)alGetProcAddress("alGetAuxiliaryEffectSlotf")) == 0);
	r |= ((alGetAuxiliaryEffectSlotfv = (LPALGETAUXILIARYEFFECTSLOTFV)alGetProcAddress("alGetAuxiliaryEffectSlotfv")) == 0);
	return !r;
}

ALboolean alewInit(ALCdevice *pDevice)
{
	// Check if the extension is present
	if(alcIsExtensionPresent(pDevice, (ALCchar*)ALC_EXT_EFX_NAME))
		ALEW_EXT_efx = _alewInit_EXT_efx();
	return AL_TRUE;
}