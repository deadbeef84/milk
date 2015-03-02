#ifndef MILK_CEFFECTGROUP_H_
#define MILK_CEFFECTGROUP_H_

#include <al.h>
#include "milk/audio/alew.h"
#include "milk/audio/sound.h"

namespace milk
{

	class IEffect
	{
		friend class CEffectGroup;
	public:
		IEffect()
		{
			alGenEffects(1, &m_effectId);
			Sound::checkForErrors();
		}

		virtual ~IEffect()
		{
			alDeleteEffects(1, &m_effectId);
		}

	protected:
		ALuint m_effectId;
	};

	class IFilter
	{
		friend class CSoundSource;
	public:
		IFilter()
		{
			alGenFilters(1, &m_filterId);
			Sound::checkForErrors();
		}

	protected:
		ALuint m_filterId;
	};

	#define EFFECT_BEGIN(n, N)\
		class CEffect##n : public IEffect {\
		public:\
			CEffect##n()\
			{ alGetError(); alEffecti(m_effectId, AL_EFFECT_TYPE, AL_EFFECT_##N); if(alGetError()!=AL_NO_ERROR) throw error::sound("Effect '" #n "' not available."); }\
			virtual ~CEffect##n()\
			{ }

	#define EFFECT_END };

	#define EFFECT_PARAM(t, T, n, N)\
		void set##n(T a) const\
		{ alEffect##t(m_effectId, AL_##N, a); }\
		T get##n() const\
		{ T a; alGetEffect##t(m_effectId, AL_##N, &a); return a; }

	#define FILTER_BEGIN(n, N)\
		class CFilter##n : public IFilter {\
		public:\
			CFilter##n()\
			{ alFilteri(m_filterId, AL_FILTER_TYPE, AL_FILTER_##N); }\
			virtual ~CFilter##n()\
			{ }

	#define FILTER_END };

	#define FILTER_PARAM(t, T, n, N)\
		void set##n(T a)\
		{ alFilter##t(m_filterId, AL_##N, a); }

	EFFECT_BEGIN(Reverb, REVERB)
	EFFECT_PARAM(f, float, Density, REVERB_DENSITY)
	EFFECT_PARAM(f, float, Diffusion, REVERB_DIFFUSION)
	EFFECT_PARAM(f, float, Gain, REVERB_GAIN)
	EFFECT_PARAM(f, float, GainHF, REVERB_GAINHF)
	EFFECT_PARAM(f, float, DecayTime, REVERB_DECAY_TIME)
	EFFECT_PARAM(f, float, DecayHFRatio, REVERB_DECAY_HFRATIO)
	EFFECT_PARAM(f, float, ReflectionsGain, REVERB_REFLECTIONS_GAIN)
	EFFECT_PARAM(f, float, ReflectionsDelay, REVERB_REFLECTIONS_DELAY)
	EFFECT_PARAM(f, float, LateReverbGain, REVERB_LATE_REVERB_GAIN)
	EFFECT_PARAM(f, float, LateReverbDelay, REVERB_LATE_REVERB_DELAY)
	EFFECT_PARAM(f, float, AirAbsorptionGainHF, REVERB_AIR_ABSORPTION_GAINHF)
	EFFECT_PARAM(f, float, RoomRolloffFactor, REVERB_ROOM_ROLLOFF_FACTOR)
	EFFECT_PARAM(i, int, DecayHFLimit, REVERB_DECAY_HFLIMIT)
	EFFECT_END

	EFFECT_BEGIN(Chorus, CHORUS)
	EFFECT_PARAM(i, int, Waveform, CHORUS_WAVEFORM)
	EFFECT_PARAM(f, float, Phase, CHORUS_PHASE)
	EFFECT_PARAM(f, float, Rate, CHORUS_RATE)
	EFFECT_PARAM(f, float, Depth, CHORUS_DEPTH)
	EFFECT_PARAM(f, float, Feedback, CHORUS_FEEDBACK)
	EFFECT_PARAM(f, float, Delay, CHORUS_DELAY)
	EFFECT_END

	EFFECT_BEGIN(Distortion, DISTORTION)
	EFFECT_PARAM(f, float, Edge, DISTORTION_EDGE)
	EFFECT_PARAM(f, float, Gain, DISTORTION_GAIN)
	EFFECT_PARAM(f, float, LowPassCutoff, DISTORTION_LOWPASS_CUTOFF)
	EFFECT_PARAM(f, float, EQCenter, DISTORTION_EQCENTER)
	EFFECT_PARAM(f, float, EQBandwidth, DISTORTION_EQBANDWIDTH)
	EFFECT_END

	EFFECT_BEGIN(Echo, ECHO)
	EFFECT_PARAM(f, float, Delay, ECHO_DELAY)
	EFFECT_PARAM(f, float, LRDelay, ECHO_LRDELAY)
	EFFECT_PARAM(f, float, Damping, ECHO_DAMPING)
	EFFECT_PARAM(f, float, Feedback, ECHO_FEEDBACK)
	EFFECT_PARAM(f, float, Spread, ECHO_SPREAD)
	EFFECT_END

	EFFECT_BEGIN(Flanger, FLANGER)
	EFFECT_PARAM(i, int, Waveform, FLANGER_WAVEFORM)
	EFFECT_PARAM(f, float, Phase, FLANGER_PHASE)
	EFFECT_PARAM(f, float, Rate, FLANGER_RATE)
	EFFECT_PARAM(f, float, Depth, FLANGER_DEPTH)
	EFFECT_PARAM(f, float, Feedback, FLANGER_FEEDBACK)
	EFFECT_PARAM(f, float, Delay, FLANGER_DELAY)
	EFFECT_END

	EFFECT_BEGIN(FrequencyShifter, FREQUENCY_SHIFTER)
	EFFECT_PARAM(f, float, Frequency, FREQUENCY_SHIFTER_FREQUENCY)
	EFFECT_PARAM(i, int, LeftDirection, FREQUENCY_SHIFTER_LEFT_DIRECTION)
	EFFECT_PARAM(i, int, RightDirection, FREQUENCY_SHIFTER_RIGHT_DIRECTION)
	EFFECT_END

	EFFECT_BEGIN(VocalMorpher, VOCAL_MORPHER)
	EFFECT_PARAM(i, int, PhonemeA, VOCAL_MORPHER_PHONEMEA)
	EFFECT_PARAM(i, int, PhonemeACoarseTuning, VOCAL_MORPHER_PHONEMEA_COARSE_TUNING)
	EFFECT_PARAM(i, int, PhonemeB, VOCAL_MORPHER_PHONEMEB)
	EFFECT_PARAM(i, int, PhonemeBCoarseTuning, VOCAL_MORPHER_PHONEMEB_COARSE_TUNING)
	EFFECT_PARAM(i, int, Waveform, VOCAL_MORPHER_WAVEFORM)
	EFFECT_PARAM(f, float, Rate, VOCAL_MORPHER_RATE)
	EFFECT_END

	EFFECT_BEGIN(PitchShifter, PITCH_SHIFTER)
	EFFECT_PARAM(i, int, CoarseTune, PITCH_SHIFTER_COARSE_TUNE)
	EFFECT_PARAM(i, int, FineTune, PITCH_SHIFTER_FINE_TUNE)
	EFFECT_END

	EFFECT_BEGIN(RingModulator, RING_MODULATOR)
	EFFECT_PARAM(f, float, Frequency, RING_MODULATOR_FREQUENCY)
	EFFECT_PARAM(f, float, HighPassCutoff, RING_MODULATOR_HIGHPASS_CUTOFF)
	EFFECT_PARAM(i, int, Waveform, RING_MODULATOR_WAVEFORM)
	EFFECT_END

	EFFECT_BEGIN(AutoWah, AUTOWAH)
	EFFECT_PARAM(f, float, AttackTime, AUTOWAH_ATTACK_TIME)
	EFFECT_PARAM(f, float, ReleaseTime, AUTOWAH_RELEASE_TIME)
	EFFECT_PARAM(f, float, Resonance, AUTOWAH_RESONANCE)
	EFFECT_PARAM(f, float, PeakGain, AUTOWAH_PEAK_GAIN)
	EFFECT_END

	EFFECT_BEGIN(Compressor, COMPRESSOR)
	EFFECT_PARAM(i, int, OnOff, COMPRESSOR_ONOFF)
	EFFECT_END
	
	EFFECT_BEGIN(Equalizer, EQUALIZER)
	EFFECT_PARAM(f, float, LowGain, EQUALIZER_LOW_GAIN)
	EFFECT_PARAM(f, float, LowCutoff, EQUALIZER_LOW_CUTOFF)
	EFFECT_PARAM(f, float, Mid1Gain, EQUALIZER_MID1_GAIN)
	EFFECT_PARAM(f, float, Mid1Center, EQUALIZER_MID1_CENTER)
	EFFECT_PARAM(f, float, Mid1Width, EQUALIZER_MID1_WIDTH)
	EFFECT_PARAM(f, float, Mid2Gain, EQUALIZER_MID2_GAIN)
	EFFECT_PARAM(f, float, Mid2Center, EQUALIZER_MID2_CENTER)
	EFFECT_PARAM(f, float, Mid2Width, EQUALIZER_MID2_WIDTH)
	EFFECT_PARAM(f, float, HighGain, EQUALIZER_HIGH_GAIN)
	EFFECT_PARAM(f, float, HighCutoff, EQUALIZER_HIGH_CUTOFF)
	EFFECT_END

	FILTER_BEGIN(LowPass, LOWPASS)
	FILTER_PARAM(f, float, Gain, LOWPASS_GAIN)
	FILTER_PARAM(f, float, GainHF, LOWPASS_GAINHF)
	FILTER_END

	FILTER_BEGIN(HighPass, HIGHPASS)
	FILTER_PARAM(f, float, Gain, HIGHPASS_GAIN)
	FILTER_PARAM(f, float, GainLF, HIGHPASS_GAINLF)
	FILTER_END

	FILTER_BEGIN(BandPass, BANDPASS)
	FILTER_PARAM(f, float, Gain, BANDPASS_GAIN)
	FILTER_PARAM(f, float, GainLF, BANDPASS_GAINLF)
	FILTER_PARAM(f, float, GainHF, BANDPASS_GAINHF)
	FILTER_END

	// Effect group, referred to as EffectSlots in the openal manual.
	class CEffectGroup
	{
		friend class CSoundSource;
	public:
		CEffectGroup()
		{
			if(!ALEW_EXT_efx)
				throw error::sound("Missing extension 'EXT_efx'");
			alGenAuxiliaryEffectSlots(1, &m_effectSlot);
			Sound::checkForErrors();
		}

		~CEffectGroup()
		{
			alDeleteAuxiliaryEffectSlots(1, &m_effectSlot);
			Sound::checkForErrors();
		}

		void setEffect(const IEffect& effect)
		{
			alAuxiliaryEffectSloti(m_effectSlot, AL_EFFECTSLOT_EFFECT, effect.m_effectId);
			Sound::checkForErrors();
		}

		void setGain(float gain)
		{
			alAuxiliaryEffectSlotf(m_effectSlot, AL_EFFECTSLOT_GAIN, gain);
		}

		void setSendAuto(bool sa)
		{
			alAuxiliaryEffectSloti(m_effectSlot, AL_EFFECTSLOT_AUXILIARY_SEND_AUTO, sa?AL_TRUE:AL_FALSE);
		}

	private:
		ALuint m_effectSlot;
	};

}

#endif