#include "milk/cimage.h"
#include "milk/error.h"
#include "milk/renderer/ctexture.h"
#include "milk/math/math.h"
#include "milk/math/crect.h"
#include "milk/helper.h"
#include <algorithm>
#include <climits>
#include <iomanip>
#include <SDL.h>
#include <SDL_image.h>
using namespace std;
using namespace milk;

CImage::CImage() :
m_size(0, 0), m_alpha(false)
{}

CImage::CImage(const string& file) :
m_size(0, 0), m_alpha(false)
{
	load(file);
}


CImage::CImage(const boost::shared_ptr<CTexture2D>& text) :
m_size(0, 0), m_alpha(false)
{
	load(text);
}

CImage::CImage(const CVector2<int>& size, const CImage::color_type& defcolor, bool alpha)
{
	load(size, defcolor, alpha);
}

void CImage::load(const string& file)
{
	if (file.size() > 4 && file.substr(file.size()-4, string::npos) == ".raw")
	{
		loadRAW(file, ALPHA);
		return;
	}

	SDL_Surface *pic = IMG_Load(file.c_str());
	if(!pic)
		throw error::file_read("Could not load image file \""+file+"\". SDL_image error: \""+IMG_GetError()+"\"");


	m_size.x = pic->w;
	m_size.y = pic->h;

	//convert to 32-bit RGBA
	if (pic->format->BitsPerPixel != 32)
	{
		SDL_Surface *temp = SDL_CreateRGBSurface(SDL_SWSURFACE, (Uint32)pic->w, (Uint32)pic->h, (Uint32)32,
			(Uint32)0xff, (Uint32)0xff<<8, (Uint32)0xff<<16, (Uint32)0xff<<24);

		SDL_BlitSurface(pic, 0, temp, 0);
		SDL_FreeSurface(pic);
		pic=temp;

		m_alpha=false;
	}
	else
	{
		m_alpha = true;
	}

	m_data.resize(m_size.x*m_size.y);

	SDL_LockSurface(pic);

	const color_type *pixels = reinterpret_cast<color_type*>(pic->pixels);
	vectorPOD<color_type>::iterator it = m_data.begin() + (m_size.y-1)*m_size.x;

	for (int i=0 ; i<m_size.y ; ++i, pixels+=m_size.x, it-=m_size.x)
		copy(pixels, pixels + m_size.x, it);

	SDL_UnlockSurface(pic);
	SDL_FreeSurface(pic);

	//32-bit targas are loaded with swapped r and b channels - fix!
	if (m_alpha && file.substr(file.size()-4, string::npos) == ".tga")
	{
		for (it = m_data.begin(); it!=m_data.end(); ++it)
			std::swap(it->r, it->b);
	}
}

void CImage::load(const boost::shared_ptr<CTexture2D>& text)
{
	text->bind();
	m_alpha	= text->getAlpha();
	m_size	= text->size();
	m_data.resize(m_size.x*m_size.y);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)getDataPointer());
}

void CImage::load(const CVector2<int>& size, const CImage::color_type& defcolor, bool alpha)
{
	m_alpha = alpha;
	m_size = size;
	m_data.resize(m_size.x*m_size.y, defcolor);
}

void CImage::loadRAW(const std::string& file, RAWFormat format, int width, int height)
{
	m_size.x = width;
	m_size.y = height;
	m_alpha = (format==ALPHA || format==RGBA);
	m_data.resize(m_size.x*m_size.y);

	FILE *fp = fopen(file.c_str(), "rb");
	if (!fp)
		throw error::file_not_found("Could not find file \""+file+"\".");

	for(vectorPOD<color_type>::iterator it=m_data.begin() ; it!=m_data.end() ; ++it)
	{
		if (format==ALPHA)
			fread(&it->a, 1, 1, fp);
		else if (format==RGB)
			fread((uchar*)*it, 3, 1, fp);
		else
			fread((uchar*)*it, 4, 1, fp);

		if (ferror(fp))
		{
			fclose(fp);
			throw error::file_read("File \""+file+"\" is corrupt.");
		}
	}

	fclose(fp);

	for (int i=0 ; i<m_size.y ; ++i)
		reverse(m_data.begin() + i*m_size.x , m_data.begin() + (i+1)*m_size.x);

	reverse(m_data.begin(), m_data.end());
}

void CImage::loadRAW(const std::string& file, RAWFormat format)
{
	FILE *fp = fopen(file.c_str(), "rb");
	if (!fp)
	{
		throw error::file_not_found("Could not find file \""+file+"\".");
	}

	if (fseek(fp, 0, SEEK_END)!=0)
	{
		fclose(fp);
		throw error::file_read("File \""+file+"\" is corrupt.");
	}
	
	int size = ftell(fp);
	
	fclose(fp);

	if (format == RGB)
	{
		size /= 3;
	}
	else if (format == RGBA)
	{
		size /= 4;
	}

	int side = static_cast<int>(math::sqrt(static_cast<float>(size)));
	if (side*side != size)
		throw error::file_read("The RAW-file \""+file+"\" contained a non quadratic picture.");
	
	loadRAW(file, format, side, side);
}

void CImage::saveRAW(const std::string& file, RAWFormat format) const
{
	FILE *fp = fopen(file.c_str(), "wb");
	if (!fp)
	{
		throw error::file_not_found("Could not open file \""+file+"\" for writing.");
	}

	for(data_type::const_iterator it=m_data.begin() ; it!=m_data.end() ; ++it)
	{
		if (format==ALPHA)
			fwrite(&it->a, 1, 1, fp);
		else if (format==RGB)
			fwrite(&it->r, 3, 1, fp);
		else
			fwrite(&it->r, 4, 1, fp);
		if (ferror(fp))
		{
			fclose(fp);
			throw error::file_read("File \""+file+"\" is corrupt.");
		}
	}
	fclose(fp);
}

void CImage::saveTGA24(const string& file) const //24-bit TGA
{
	FILE* fp;
	fp=fopen(file.c_str(), "wb");
	if (!fp)
	{
		throw error::file_write("Could not write to file \""+file+"\".");
	}

	Uint8 header[18] =
	{
		0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		Uint8(m_size.x&0x00ff),
		Uint8((m_size.x&0xff00)/256),
		Uint8(m_size.y&0x00ff),
		Uint8((m_size.y&0xff00)/256),
		25,
		0
	};

	fwrite(header, 18, 1, fp);

	for(data_type::const_iterator it=m_data.begin() ; it!=m_data.end() ; ++it)
	{
		fwrite(&*it, 1, 3, fp);
	}

	fclose(fp);
}

void CImage::createMask()
{
	for(vectorPOD<color_type>::iterator it=m_data.begin() ; it!=m_data.end() ; ++it)
		it->a = (uchar)math::roundOff<float>( 0.2125f * it->r  +  0.7154f * it->g  +  0.0721f * it->b );
	m_alpha = true;
}

void CImage::setAlpha(const CImage& img)
{
	if (m_data.size() != img.m_data.size())
		throw runtime_error("The argument of CImage::setAlpha didnt have the same size.");

	for(uint i=0 ; i<m_data.size() ; ++i)
		m_data[i].a = img.m_data[i].a;
}

void CImage::screen_capture()
{
	int screenStats[4];
	glGetIntegerv(GL_VIEWPORT, screenStats);
	CImage::screen_capture(CRect<int>(0, 0, screenStats[2], screenStats[3]));
}

void CImage::screen_capture(const CRect<int>& rect)
{
	m_size.x = rect.getW();
	m_size.y = rect.getH();
	m_data.resize(m_size.x*m_size.y);
	m_alpha=false;
	glReadPixels(rect.x1, rect.x2, m_size.x, m_size.y,
				GL_RGBA, GL_UNSIGNED_BYTE, getDataPointer());
}

void CImage::resize(const CVector2<int>& size)
{
	m_data.resize(size.x*size.y);
	m_size = size;
}

void CImage::scale(const CVector2<int>& size)
{
	if (m_size == size)
		return;

	data_type out(size.x*size.y);
	if (m_size.x==0 || m_size.y==0)
		fill(out.begin(), out.end(), color_type());
	else
		gluScaleImage(GL_RGBA, m_size.x, m_size.y, GL_UNSIGNED_BYTE, (void*)getDataPointer(),
		size.x, size.y, GL_UNSIGNED_BYTE, (void*)&out.front());
	out.swap(m_data);
	m_size = size;
}

void CImage::scaleToTexture(int snap)
{
	// Texture should be n^2
	static const double ln2 = math::log(2.0);

	double wn = math::log((double)m_size.x)/ln2;
	double hn = math::log((double)m_size.y)/ln2;
	if (snap < 0)
	{
		wn = floor(wn);
		hn = floor(hn);
	}
	else if (snap > 0)
	{
		wn = ceil(wn);
		hn = ceil(hn);
	}
	else
	{
		wn = math::roundOff(wn);
		hn = math::roundOff(hn);
	}
	int wnew = (int)math::pow(2.0, wn);
	int hnew = (int)math::pow(2.0, hn);
	if (wnew != m_size.x || hnew != m_size.y)
		scale(CVector2<int>(wnew, hnew));
}

void CImage::setAlpha(bool alpha)
{
	m_alpha = alpha;
}

void milk::screenShotToFile(const std::string& filename)
{
	/*
	CImage img;
	img.screen_capture();
	img.saveTGA24(filename);
	*/

	static vector<Uint8> pixels;
	static int width=0, height=0, size=0;

	int screenStats[4];
	glGetIntegerv(GL_VIEWPORT, screenStats);
	if (screenStats[2]*screenStats[3] != size)
	{
		width = screenStats[2];
		height = screenStats[3];
		size = width*height*3;
		pixels.resize(size);
	}

	glReadPixels(0, 0, width, height, GL_BGR_EXT, GL_UNSIGNED_BYTE, &pixels[0]);

	FILE *fp=fopen(filename.c_str(), "wb");
	if(!fp)
		throw error::file_write("Could not write to file \""+filename+"\".");

	Uint8 header[18] =
	{
		0,0,2,0,0,0,0,0,0,0,0,0,
		Uint8(width%256),
		Uint8(width/256),
		Uint8(height%256),
		Uint8(height/256),
		24, 0
	};

	fwrite(header, 1, 18, fp);
	fwrite(&pixels[0], 1, size, fp);
	fclose(fp);
}

void milk::screenShot(const std::string& nameBegin)
{ 
	string name("to_many_screenshots.tga");

	//first we find a name for the file that does not already exist
	for (int i=0 ; i<=9999 ; i++)
	{
		ostringstream stream;
		stream << nameBegin << setfill('0') << setw(4) << i << ".tga";
		
		if (!fileExists(stream.str()))
		{
			name = stream.str();
			break;
		}
	}

	screenShotToFile(name);
}
