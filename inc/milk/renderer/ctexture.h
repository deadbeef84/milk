#ifndef MILK_CTEXTURE_H_
#define MILK_CTEXTURE_H_

#include <string>
#include <algorithm>
#include <stack>
#include "milk/includes.h"
#include "milk/types.h"
#include "milk/math/cvector.h"
#include "milk/ccolor.h"
#include "milk/renderer/irendertarget.h"
#include "milk/iresource.h"
#include "milk/error.h"

namespace milk
{
	enum WrapMethod		{ Repeat, Clamp, ClampToBorder };
	enum FilterMethod	{ Nearest, Linear };
	enum TextureFormat	{ DEPTH16, DEPTH24, DEPTH32,
						  A8,
						  RGB16, RGBA16, RGB24, RGBA32 };

	class CCubeMap;

	/// Describer of texture-quality
	class Quality
	{
	public:
		Quality(WrapMethod a_wrap_s=Repeat, WrapMethod a_wrap_t=Repeat,
			FilterMethod a_min_filter=Linear, FilterMethod a_mag_filter=Linear,
			FilterMethod a_mipmap_filter=Linear)
			:	wrap_s(a_wrap_s), wrap_t(a_wrap_t),
				min_filter(a_min_filter), mag_filter(a_mag_filter), mipmap_filter(a_mipmap_filter)
		{
		}

		WrapMethod wrap_s, wrap_t;
		FilterMethod min_filter, mag_filter, mipmap_filter;
	};

	/// Describer of image, bit depth, mipmaping and quality.
	class ImageType
	{
	public:
		explicit ImageType(TextureFormat a_type=RGBA32,
			bool a_mipmap=true,
			const Quality& a_quality=Quality())
		: type(a_type), mipmap(a_mipmap), quality(a_quality)
		{
		}

		TextureFormat type;
		bool mipmap;
		Quality quality;
	};

	/// TODO: Incomplete
	class ITexture : public IResource
	{
	public:
		ITexture(GLenum target)
			: m_id(0), m_target(target)
		{ }

		virtual ~ITexture();

		MILK_MAKE_RESOURCE(ITexture);

		static ITexture* create(const std::string& rid);

		void bind() const
		{ glBindTexture(m_target, m_id); }

		void unbind()
		{ glBindTexture(m_target, 0); }

		GLenum getTarget() const
		{ return m_target; }

		GLuint getTextureId() const
		{ return m_id; }

		bool getAlpha() const
		{ return (m_imgType.type == RGBA32 || m_imgType.type == RGBA16 || m_imgType.type == A8); }

		bool isDepthTexture() const
		{ return (m_imgType.type == DEPTH16 || m_imgType.type == DEPTH24); }

		const ImageType& getImageType() const
		{ return m_imgType; }

		const Quality& getQuality() const
		{ return m_imgType.quality; }

		void setQuality(const Quality& quality);

		void setBorderColor(const CColor4f& color);

		//////////////////////////////////////////////////////////////////////////

		static int toInternalGLFormat(TextureFormat f);
		static int toGLFormat(TextureFormat f);
		static GLenum getTargetBinding(GLenum target);

		static void activateUnit(int unit);
		static void bind(int unit, ITexture& texture);
		static void disable(int unit);
		static void reset();

	protected:
		enum {NUM_TARGET_TYPES=4};

		static int getTargetIndex(GLenum target);

		GLuint m_id;
		GLenum m_target;

		ImageType m_imgType;

		static int ms_activeUnit;
		static const GLenum ms_targetName[NUM_TARGET_TYPES];
		static GLuint ms_targetBinding[8][NUM_TARGET_TYPES];
		static bool ms_targetEnabled[8][NUM_TARGET_TYPES];
	};

	/// TODO: Incomplete
	class CTexture1D : public ITexture
	{
	public:
		CTexture1D()
			: ITexture(GL_TEXTURE_1D)
		{}
		virtual ~CTexture1D() {}

		const int size() const
		{ return m_size; }

		static void unbind()
		{ glBindTexture(GL_TEXTURE_1D, 0); }

	protected:
		int m_size;
	};

	/// Class for loading, containing and using 2-dimensional textures
	class CTexture2D : public ITexture
	{
	public:
		/// create texture from a CImage
		explicit CTexture2D(const class CImage&, const ImageType& imgtype=ImageType());

		/// use the textured specified in the memory at position "mem", with the format "inFormat".
		CTexture2D(const uchar* mem, const CVector2<int>& size,
			TextureFormat inFormat=RGBA32, const ImageType& imgtype=ImageType());

		/// Create empty texture
		CTexture2D(const CVector2<int>& size, const ImageType& imgtype=ImageType());

		/*
		/// returns the default openGL texture
		CTexture2D();
		*/

		/// returns the default openGL texture
		CTexture2D(CCubeMap& cubemap, GLenum face);

		/// destructor
		virtual ~CTexture2D()
		{ }

		GLenum getFaceTarget() const
		{ return m_faceTarget; }

		/// get the size of the window
		const CVector2<int>& size() const
		{ return m_size; }

		void setShadowMap();

		static void unbind()
		{ glBindTexture(GL_TEXTURE_2D, 0); }

	protected:
		void private_ctor(const uchar* mem, const CVector2<int>& size, TextureFormat inFormat, const ImageType& imgtype);
		
		CVector2<int> m_size;

		handle<CCubeMap> m_cubeMap;
		GLenum m_faceTarget;

		CTexture2D(const CTexture2D&);
		CTexture2D& operator=(const CTexture2D&);
	};

	/// TODO: Incomplete
	class CTexture3D : public ITexture
	{
	public:
		CTexture3D()
			: ITexture(GL_TEXTURE_2D)
		{ }

		virtual ~CTexture3D()
		{ }

		const CVector3<int>& size() const
		{ return m_size; }

		static void unbind()
		{ glBindTexture(GL_TEXTURE_3D, 0); }

	protected:
		CVector3<int> m_size;
	};

	/// TODO: Incomplete
	class CCubeMap : public ITexture
	{
	public:
		CCubeMap();
		virtual ~CCubeMap();

		handle<CTexture2D> getFace(int face);

		static void unbind()
		{ glBindTexture(GL_TEXTURE_CUBE_MAP, 0); }

	protected:
		GLuint m_id;
	};

	class binder
	{
	public:
		binder(ITexture& tex)
			: m_tex(tex)
		{
			glGetIntegerv(ITexture::getTargetBinding(tex.getTarget()), &m_id);
			m_tex.bind();
		}

		~binder()
		{
			glBindTexture(m_tex.getTarget(), m_id);
		}

	private:
		MILK_NO_COPY(binder);

		ITexture& m_tex;
		GLint m_id;
	};
}

#endif
