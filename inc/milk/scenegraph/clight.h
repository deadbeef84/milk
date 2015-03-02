#ifndef MILK_CLIGHT_H_
#define MILK_CLIGHT_H_

// Includes
///////////////////////////////////////////////

#include "milk/math/cvector.h"
#include "milk/scenegraph/iscenenode.h"
#include "milk/types.h"
#include "milk/ccolor.h"

namespace milk
{
	/// Light emitting entity
	/**
		There are three different types of light sources.
		DIRECTIONAL, OMNI and SPOT.

		Omni
			Omni(directional), aka point light casts equal amount
			of light in all directions.

		Spot	
			Spot lights casts light with the shape of a cone. It
			have an exponent and cutoff property to control the
			shape of the cone.
		
		Directional
			This is light coming from an infinite distant emitter.
	*/
	class CLight : public ISceneNode
	{
	public:
		enum LightType
		{
			DIRECTIONAL,
			OMNI,
			SPOT
		};

		CLight(LightType type=DIRECTIONAL);
		virtual ~CLight();

		/// Enable light.
		void enable();

		/// Disable light.
		void disable();

		/// Checks whether this light is enabled or not.
		bool isEnabled();

		/// Set the type of this light source.
		void setType(LightType type)
		{ m_type = type; }
		/// Get the type of this light source.
		LightType getType() const
		{ return m_type; }

		/// Set light intensity.
		void setIntensity(float intensity)
		{ m_intensity = intensity; }
		/// Get light intensity.
		float getIntensity() const
		{ return m_intensity; }

		/// Set ambient light color.
		void setAmbient(const CColor4f& ambient)
		{ m_ambient = ambient; }
		/// Get ambient light color.
		const CColor4f& getAmbient() const
		{ return m_ambient; }

		/// Set diffuse light color.
		void setDiffuse(const CColor4f& diffuse)
		{ m_diffuse = diffuse; }
		/// Get diffuse light color.
		const CColor4f& getDiffuse() const
		{ return m_diffuse; }

		/// Set specular light color.
		void setSpecular(const CColor4f& specular)
		{ m_specular = specular; }
		/// Get the specular light color.
		const CColor4f& getSpecular() const
		{ return m_specular; }

		/// Set spotlight cutoff angle. [SPOTLIGHT only]
		/**
			From the OpenGL manual:
			Only values in the range [0,90], and the special value 180, are accepted.
			If the angle between the direction of the light and the direction from
			the light to the vertex being lighted is greater than the spot cutoff
			angle,then the light is completely masked. Otherwise, its intensity is
			controlled by the spot exponent and the attenuation factors. The default
			spot cutoff is 180, resulting in uniform light distribution.
		*/
		void setCutoff(float cutoff)
		{ m_cutoff = cutoff; }

		/// Set spotlight exponent. [SPOTLIGHT only]
		/**
			Specify the intensity distribution of the light
			From the OpenGL manual:
			Only values in the range [0,128] are accepted.
			Effective light intensity is attenuated by the cosine of the angle
			between the direction of the light and the direction from the light to
			the vertex being lighted, raised to the power of the spot exponent.
			Thus, higher spot exponents result in a more focused light source,
			regardless of the spot cutoff angle. The default spot exponent is 0,
			resulting in uniform light distribution.
		*/
		void setExponent(float exponent)
		{ m_exponent = exponent; }

		/// Set light attenuation factors
		/**
			This have the same effect as the light-attenuation factor in OpenGL which
			is described like this: "Only nonnegative values are accepted. If the
			light is positional, rather than directional, its intensity is attenuated
			by the reciprocal of the sum of: the constant factor, the linear factor
			multiplied by the distance between the light and the vertex being lighted,
			and the quadratic factor multiplied by the square of the same distance. The
			default attenuation factors are (1,0,0), resulting in no attenuation."
			\param	constant	The constant attenuation value (default 1)
			\param	linear		The linear attenuation value (default 0)
			\param	quadratic	The quadratic attenuation value (default 0)
		*/
		void setAttenuation(float constant, float linear, float quadtratic)
		{ m_attConstant = constant; m_attLinear = linear; m_attQuadtratic = quadtratic; }

		/// Set light constant attenuation factor. See setAttenuation for details.
		void setConstantAttenuation(float constant)
		{ m_attConstant = constant; }
		/// Get light constant attenuation factor.
		float getConstantAttenuation() const
		{ return m_attConstant; }

		/// Set light linear attenuation factor. See setAttenuation for details.
		void setLinearAttenuation(float linear)
		{ m_attLinear = linear; }
		/// Get light linear attenuation factor.
		float getLinearAttenuation() const
		{ return m_attLinear; }

		/// Set light quadratic attenuation factor. See setAttenuation for details.
		void setQuadraticAttenuation(float quadtratic)
		{ m_attQuadtratic = quadtratic; }
		/// Get light quadratic attenuation factor.
		float getQuadraticAttenuation() const
		{ return m_attQuadtratic; }

		/// Apply light.
		/**
			This function should be called every time the camera is moved or when 
			light properties are changed.
		*/
		void apply();

		/// Set the global ambient color.
		static void setGlobalAmbient(const CColor4f& ambient);

		/// Get the maximum numbers of active lights.
		static uint getMaxLights();

		/// Get a free light (opengl identifier i, GL_LIGHTi), -1 if none are free.
		static int getFreeLight();

		/// Disables all opengl lights
		static void disableAll();

	protected:
		void onNewSceneManager(CSceneManager *pSceneManager);

		int m_id;
		LightType m_type;

		float m_cutoff, m_exponent;

		float m_intensity;
		CColor4f m_ambient, m_diffuse, m_specular;
		float m_attConstant, m_attLinear, m_attQuadtratic;
	};
}

#endif
