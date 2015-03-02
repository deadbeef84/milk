#ifndef MILK_CIMAGE_H_
#define MILK_CIMAGE_H_

/* image-class for opengl suing sdl */

#include <vector>
#include <string>
#include <algorithm>
#include "milk/types.h"
#include "milk/ccolor.h"
#include "milk/vectorpod.h"
#include "milk/math/cvector.h"
#include "milk/math/crect.h"
#include "milk/boost.h"

namespace milk
{
	template<class T>
	class CRect;

	class CTexture2D;

	/// Image-class for storing and manipulating images
	class CImage
	{
		typedef CColor4ub color_type;
		typedef vectorPOD<color_type> data_type;

	public:
		CImage();
		explicit CImage(const std::string& file);
		explicit CImage(const boost::shared_ptr<CTexture2D>&);
		CImage(const CVector2<int>& size, const color_type& defcolor=color_type(), bool alpha=false);

		/// loads file with SDL_image, or loadRAW(file) if raw
		void load(const std::string& file);
		void load(const boost::shared_ptr<CTexture2D>&);
		void load(const CVector2<int>& size, const color_type& defcolor=color_type(), bool alpha=false);

		enum RAWFormat
		{
			ALPHA,	//reads 1 uchar at the time to alpha
			RGB,	//reads 3 bytes at the time to r, g and b
			RGBA	//reads 4 bytes at the time to r, g, b and a
		};

		/// loads a RAW
		void loadRAW(const std::string& file, RAWFormat format, int width, int height);

		/// same as above, but quadratic RAW is assumed. error::file_read thrown if not
		void loadRAW(const std::string& file, RAWFormat format);

		/// save to raw file
		void saveRAW(const std::string& file, RAWFormat format) const;

		/// Save to TGA file
		void saveTGA24(const std::string& file) const; //24-bit TGA

		/// create an alpha-mask
		void createMask();

		/* asign the alpha from another
		image to the alpha channel of this img */
		void setAlpha(const CImage&);

		///////////////////////////////////////////////////////////////7

		void screen_capture();
		void screen_capture(const CRect<int>&);

		///////////////////////////////////////////////////////////////7

		/// wont save your old iamge data
		void resize(const CVector2<int>& size);

		/// scale the image to a new size
		void scale(const CVector2<int>& size);

		/// scale it to a width and height that both are 2^n
		void scaleToTexture(int snap = 0);

		void setAlpha(bool alpha);

		const CVector2<int>& size() const
		{ return m_size; }

		bool alpha() const
		{ return m_alpha; }

		CRect<int> rect() const
		{ return CRect<int>(CVector2<int>(0,0), m_size); }

		color_type& pixel(int x, int y)
		{ return m_data[y*m_size.x+x]; }
		color_type pixel(int x, int y) const
		{ return m_data[y*m_size.x+x]; }

		friend void swap(CImage& x, CImage& y)
		{
			std::swap(x.m_alpha,	y.m_alpha);
			std::swap(x.m_size,		y.m_size);
			x.m_data.swap(y.m_data);
		}

		color_type *getDataPointer()
		{ return &m_data.front(); }

		const color_type *getDataPointer() const
		{ return &m_data.front(); }

	private:
		data_type m_data;
		CVector2<int> m_size;
		bool m_alpha;
	};

	void screenShotToFile(const std::string& filename);
	void screenShot(const std::string& nameBegin);
}


#endif
