#ifndef MILK_GLHELPER_H_
#define MILK_GLHELPER_H_

#include "milk/includes.h"
#include "milk/ccolor.h"
#include "milk/math/cvector.h"
#include "milk/math/cmatrix.h"
#include "milk/math/crect.h"
#include <string>

#define CHECK_FOR_OPENGL_ERRORS(x)\
	try\
	{ Renderer::checkForErrors(); }\
	catch(error::opengl& glerr)\
	{ throw error::opengl("OpenGL error in: "__FUNCTION__ "\n" x "\n\n"+(std::string)glerr.what(), glerr.getGLError()); }

namespace milk
{
	template<class T>
	void drawRect(const CRect<T>& r, bool texCoord = true, GLenum mode = GL_QUADS)
	{
		glBegin(mode);
		if(texCoord) glTexCoord2(0.0f, 1.0f); glVertex2(r.x1, r.y1);
		if(texCoord) glTexCoord2(0.0f, 0.0f); glVertex2(r.x1, r.y2);
		if(texCoord) glTexCoord2(1.0f, 0.0f); glVertex2(r.x2, r.y2);
		if(texCoord) glTexCoord2(1.0f, 1.0f); glVertex2(r.x2, r.y1);
		if(mode == GL_LINE_STRIP) glVertex2(r.x1, r.y1);
		glEnd();
	}

	/*
	template<class T>
	void drawRectRotated(const CRect<T>& r, T rotate, bool texCoord = true, GLenum mode = GL_QUADS)
	{
		glTranslate(r.getCenter());
		glRotated(rotate, 0, 0, 1);

		T w = (r.x2-r.x1)/2;
		T h = (r.y2-r.y1)/2;

		glBegin(mode);
		if(texCoord) glTexCoord2(0.0f, 1.0f); glVertex2(-w,	-h);
		if(texCoord) glTexCoord2(0.0f, 0.0f); glVertex2(-w,	+h);
		if(texCoord) glTexCoord2(1.0f, 0.0f); glVertex2(+w,	+h);
		if(texCoord) glTexCoord2(1.0f, 1.0f); glVertex2(+w,	-h);
		if(mode == GL_LINE_STRIP) glVertex2(-w, -h);
		glEnd();

		glRotated(-rotate, 0, 0, 1);
		glTranslate(-r.getCenter());
	}
	*/

	// OpenGL stuff
	// These will create template with specializations for: float, int, short and double as default
	#define MILK_OVERLOAD_GL1(name) template<class T> inline void name(T x) { name##d(x); } template<> inline void name(GLfloat x) { name##f(x); } template<> inline void name(GLint x) { name##i(x); } template<> inline void name(GLshort x) { name##s(x); }
	#define MILK_OVERLOAD_GL2(name) template<class T> inline void name(T x, T y) { name##d(x, y); } template<> inline void name(GLfloat x, GLfloat y) { name##f(x, y); } template<> inline void name(GLint x, GLint y) { name##i(x, y); } template<> inline void name(GLshort x, GLshort y) { name##s(x, y); } template<class T> inline void name(const milk::CVector2<T>& v) { name##v(reinterpret_cast<const T*>(&v)); }
	#define MILK_OVERLOAD_GL3(name) template<class T> inline void name(T x, T y, T z) { name##d(x, y, z); } template<> inline void name(GLfloat x, GLfloat y, GLfloat z) { name##f(x, y, z); } template<> inline void name(GLint x, GLint y, GLint z) { name##i(x, y, z); } template<> inline void name(GLshort x, GLshort y, GLshort z) { name##s(x, y, z); } template<class T> inline void name(const milk::CVector3<T>& v) { name##v(reinterpret_cast<const T*>(&v)); }
	#define MILK_OVERLOAD_GL4(name) template<class T> inline void name(T x, T y, T z, T w) { name##d(x, y, z, w); } template<> inline void name(GLfloat x, GLfloat y, GLfloat z, GLfloat w) { name##f(x, y, z,w); } template<> inline void name(GLint x, GLint y, GLint z, GLint w) { name##i(x, y, z, w); } template<> inline void name(GLshort x, GLshort y, GLshort z, GLshort w) { name##s(x, y, z, w); } template<class T> inline void name(const milk::CVector4<T>& v) { name##v(reinterpret_cast<const T*>(&v)); }
	#define MILK_OVERLOAD_GLV(name) template<class T> inline void name##v(const T* x) { throw error::milk(#name ": Unsupported format"); } template<> inline void name##v(const GLfloat* x) { name##fv(x); } template<> inline void name##v(const GLdouble* x) { name##dv(x); } template<> inline void name##v(const GLint* x) { name##iv(x); } template<> inline void name##v(const GLshort* x) { name##sv(x); }

	MILK_OVERLOAD_GLV(glVertex2);
	MILK_OVERLOAD_GLV(glVertex3);
	MILK_OVERLOAD_GLV(glVertex4);

	MILK_OVERLOAD_GL2(glVertex2);
	MILK_OVERLOAD_GL3(glVertex3);
	MILK_OVERLOAD_GL4(glVertex4);

	MILK_OVERLOAD_GLV(glTexCoord1);
	MILK_OVERLOAD_GLV(glTexCoord2);
	MILK_OVERLOAD_GLV(glTexCoord3);
	MILK_OVERLOAD_GLV(glTexCoord4);

	MILK_OVERLOAD_GL1(glTexCoord1);
	MILK_OVERLOAD_GL2(glTexCoord2);
	MILK_OVERLOAD_GL3(glTexCoord3);
	MILK_OVERLOAD_GL4(glTexCoord4);

	// Redefine to fit glNormal (eg. add uchar-specialization)
	#undef MILK_OVERLOAD_GL3
	#undef MILK_OVERLOAD_GLV
	#define MILK_OVERLOAD_GL3(name) template<class T> inline void name(T x, T y, T z) { name##d(x, y, z); } template<> inline void name(GLfloat x, GLfloat y, GLfloat z) { name##f(x, y, z); } template<> inline void name(GLint x, GLint y, GLint z) { name##i(x, y, z); } template<> inline void name(GLshort x, GLshort y, GLshort z) { name##s(x, y, z); } template<> inline void name(GLbyte x, GLbyte y, GLbyte z) { name##b(x, y, z); } template<class T> inline void name(const milk::CVector3<T>& v) { name##v(reinterpret_cast<const T*>(&v)); }
	#define MILK_OVERLOAD_GLV(name) template<class T> inline void name##v(const T* x) { throw error::milk(#name ": Unsupported format"); } template<> inline void name##v(const GLdouble* x) { name##dv(x); } template<> inline void name##v(const GLfloat* x) { name##fv(x); } template<> inline void name##v(const GLint* x) { name##iv(x); } template<> inline void name##v(const GLshort* x) { name##sv(x); } template<> inline void name##v(const GLbyte* x) { name##bv(x); }

	MILK_OVERLOAD_GL3(glNormal3);
	MILK_OVERLOAD_GLV(glNormal3);

	// Redefine to fit glColor (eg. specialization of these: uchar, float, int, short, unsigned uchar, unsigned int, unsigned short, with double as default)
	#undef MILK_OVERLOAD_GL3
	#undef MILK_OVERLOAD_GL4
	#undef MILK_OVERLOAD_GLV
	#define MILK_OVERLOAD_GL3(name) template<class T> inline void name(T r, T g, T b) { name##d(r, g, b); } template<> inline void name(GLbyte r, GLbyte g, GLbyte b) { name##b(r, g, b); } template<> inline void name(GLfloat r, GLfloat g, GLfloat b) { name##f(r, g, b); } template<> inline void name(GLint r, GLint g, GLint b) { name##i(r, g, b); } template<> inline void name(GLshort r, GLshort g, GLshort b) { name##s(r, g, b); } template<> inline void name(GLubyte r, GLubyte g, GLubyte b) { name##ub(r, g, b); } template<> inline void name(GLuint r, GLuint g, GLuint b) { name##ui(r, g, b); } template<> inline void name(GLushort r, GLushort g, GLushort b) { name##us(r, g, b); } template <class C> inline void name(const CColor3<C>& c) { name##v((const C*)c); }
	#define MILK_OVERLOAD_GL4(name) template<class T> inline void name(T r, T g, T b, T a) { name##d(r, g, b, a); } template<> inline void name(GLbyte r, GLbyte g, GLbyte b, GLbyte a) { name##b(r, g, b, a); } template<> inline void name(GLfloat r, GLfloat g, GLfloat b, GLfloat a) { name##f(r, g, b, a); } template<> inline void name(GLint r, GLint g, GLint b, GLint a) { name##i(r, g, b, a); } template<> inline void name(GLshort r, GLshort g, GLshort b, GLshort a) { name##s(r, g, b, a); } template<> inline void name(GLubyte r, GLubyte g, GLubyte b, GLubyte a) { name##ub(r, g, b, a); } template<> inline void name(GLuint r, GLuint g, GLuint b, GLuint a) { name##ui(r, g, b, a); } template<> inline void name(GLushort r, GLushort g, GLushort b, GLushort a) { name##us(r, g, b, a); } template <class C> inline void name(const CColor4<C>& c) { name##v((const C*)c); }
	#define MILK_OVERLOAD_GLV(name) template<class T> inline void name##v(const T* x) { throw error::milk(#name ": Unsupported format"); } template<> inline void name##v(const GLbyte* x) { name##bv(x); } template<> inline void name##v(const GLfloat* x) { name##fv(x); } template<> inline void name##v(const GLdouble* x) { name##dv(x); } template<> inline void name##v(const GLint* x) { name##iv(x); } template<> inline void name##v(const GLshort* x) { name##sv(x); } template<> inline void name##v(const GLubyte* x) { name##ubv(x); } template<> inline void name##v(const GLuint* x) { name##uiv(x); } template<> inline void name##v(const GLushort* x) { name##usv(x); } 

	MILK_OVERLOAD_GL3(glColor3);
	MILK_OVERLOAD_GLV(glColor3);
	MILK_OVERLOAD_GL4(glColor4);
	MILK_OVERLOAD_GLV(glColor4);

	#undef MILK_OVERLOAD_GL1
	#undef MILK_OVERLOAD_GL2
	#undef MILK_OVERLOAD_GL3
	#undef MILK_OVERLOAD_GL4
	#undef MILK_OVERLOAD_GLV

	/*

	void Uniform{1234}fARB(int location, T value)
	void Uniform{1234}iARB(int location, T value)
	void Uniform{1234}fvARB(int location, sizei count, T value)
	void Uniform{1234}ivARB(int location, sizei count, T value)
	void UniformMatrix(234}fvARB(int location, sizei count, boolean transpose, T value)

	*/

	inline void glUniform(GLint location, GLint i)
	{ glUniform1iARB(location, i); }

	inline void glUniform(GLint location, GLfloat f)
	{ glUniform1fARB(location, f); }

	inline void glUniform(GLint location, const CVector2f& v)
	{ glUniform2fARB(location, v.x, v.y); }

	inline void glUniform(GLint location, const CVector3f& v)
	{ glUniform3fvARB(location, 1, reinterpret_cast<const float*>(&v)); }

	inline void glUniform(GLint location, const CVector4f& v)
	{ glUniform4fvARB(location, 1, reinterpret_cast<const float*>(&v)); }

	inline void glUniform(GLint location, const CColor3f& cf)
	{ glUniform3fvARB(location, 1, (const float*)cf); }

	inline void glUniform(GLint location, const CColor4f& cf)
	{ glUniform4fvARB(location, 1, (const float*)cf); }

	inline void glUniform(GLint location, const CMatrix3f& m)
	{ glUniformMatrix3fvARB(location, 1, GL_FALSE, reinterpret_cast<const float*>(&m)); }

	inline void glUniform(GLint location, const CMatrix4f& m)
	{ glUniformMatrix4fvARB(location, 1, GL_FALSE, reinterpret_cast<const float*>(&m)); }

	/////////////////////////////////////////////////////////////////

	inline void glVertexAttrib(GLint index, GLshort i)
	{ glVertexAttrib1sARB(index, i); }

	inline void glVertexAttrib(GLint index, GLfloat f)
	{ glVertexAttrib1fARB(index, f); }

	inline void glVertexAttrib(GLint index, const CVector2f& v)
	{ glVertexAttrib2fARB(index, v.x, v.y); }

	inline void glVertexAttrib(GLint index, const CVector3f& v)
	{ glVertexAttrib3fvARB(index, reinterpret_cast<const float*>(&v)); }

	inline void glVertexAttrib(GLint index, const CVector4f& v)
	{ glVertexAttrib4fvARB(index, reinterpret_cast<const float*>(&v)); }

	inline void glVertexAttrib(GLint index, const CColor3f& cf)
	{ glVertexAttrib3fvARB(index, (const float*)cf); }

	inline void glVertexAttrib(GLint index, const CColor4f& cf)
	{ glVertexAttrib4fvARB(index, (const float*)cf); }

	inline void glVertexAttrib(GLint index, const CMatrix3f& m)
	{
		glVertexAttrib3fvARB(index,   (const float*)&m.m00);
		glVertexAttrib3fvARB(index+1, (const float*)&m.m01);
		glVertexAttrib3fvARB(index+2, (const float*)&m.m02);
	}

	inline void glVertexAttrib(GLint index, const CMatrix4f& m)
	{
		glVertexAttrib4fvARB(index,   (const float*)&m.m00);
		glVertexAttrib4fvARB(index+1, (const float*)&m.m01);
		glVertexAttrib4fvARB(index+2, (const float*)&m.m02);
		glVertexAttrib4fvARB(index+3, (const float*)&m.m03);
	}

	/////////////////////////////////////////////////////////////////

	inline void glTranslate(const CVector3f& v)
	{ glTranslatef(v.x, v.y, v.z); }

	inline void glTranslate(const CVector3d& v)
	{ glTranslated(v.x, v.y, v.z); }

	inline void glTranslate(const CVector2f& v)
	{ glTranslatef(v.x, v.y, 0.0f); }

	inline void glTranslate(const CVector2d& v)
	{ glTranslated(v.x, v.y, 0.0); }

	// Matrix
	inline void glGetMatrix(CMatrix4f& mat, GLint matrixMode = GL_MODELVIEW_MATRIX)
	{ glGetFloatv(matrixMode, reinterpret_cast<GLfloat*>(&mat)); }

	inline void glGetMatrix(CMatrix4d& mat, GLint matrixMode = GL_MODELVIEW_MATRIX)
	{ glGetDoublev(matrixMode, reinterpret_cast<GLdouble*>(&mat)); }

	// Matrix
	inline CMatrix4f glGetMatrixf(GLint matrixMode = GL_MODELVIEW_MATRIX)
	{ CMatrix4f mat; glGetFloatv(matrixMode, reinterpret_cast<GLfloat*>(&mat)); return mat; }

	inline CMatrix4d glGetMatrixd(GLint matrixMode = GL_MODELVIEW_MATRIX)
	{ CMatrix4d mat; glGetDoublev(matrixMode, reinterpret_cast<GLdouble*>(&mat)); return mat; }

	inline void glLoadMatrix(const CMatrix4f& mat)
	{ glLoadMatrixf(reinterpret_cast<const GLfloat*>(&mat)); }

	inline void glLoadMatrix(const CMatrix4d& mat)
	{ glLoadMatrixd(reinterpret_cast<const GLdouble*>(&mat)); }

	inline void glMultMatrix(const CMatrix4f& mat)
	{ glMultMatrixf(reinterpret_cast<const GLfloat*>(&mat)); }

	inline void glMultMatrix(const CMatrix4d& mat)
	{ glMultMatrixd(reinterpret_cast<const GLdouble*>(&mat)); }

	//////////////////////////////////////////////////////////////////////////
	
	template<unsigned N> struct glSizeMap
	{ enum { value = 0 }; };

	template<> struct glSizeMap<GL_BYTE>
	{ enum { value = sizeof(GLbyte) }; };

	template<> struct glSizeMap<GL_UNSIGNED_BYTE>
	{ enum { value = sizeof(GLubyte) }; };

	template<> struct glSizeMap<GL_SHORT>
	{ enum { value = sizeof(GLshort) }; };

	template<> struct glSizeMap<GL_UNSIGNED_SHORT>
	{ enum { value = sizeof(GLushort) }; };

	template<> struct glSizeMap<GL_INT>
	{ enum { value = sizeof(GLint) }; };

	template<> struct glSizeMap<GL_UNSIGNED_INT>
	{ enum { value = sizeof(GLuint) }; };

	template<> struct glSizeMap<GL_FLOAT>
	{ enum { value = sizeof(GLfloat) }; };

	template<> struct glSizeMap<GL_DOUBLE>
	{ enum { value = sizeof(GLdouble) }; };



	template<class T> struct glType
	{ enum { type = 0 }; enum { components = 1 }; enum { size = 0 }; };

	template<> struct glType<GLbyte>
	{ enum { type = GL_BYTE }; enum { components = 1 }; enum { size = sizeof(GLbyte) }; };

	template<> struct glType<GLubyte>
	{ enum { type = GL_UNSIGNED_BYTE }; enum { components = 1 }; enum { size = sizeof(GLubyte) }; };

	template<> struct glType<GLshort>
	{ enum { type = GL_SHORT }; enum { components = 1 }; enum { size = sizeof(GLshort) }; };

	template<> struct glType<GLushort>
	{ enum { type = GL_UNSIGNED_SHORT }; enum { components = 1 }; enum { size = sizeof(GLushort) }; };

	template<> struct glType<GLint>
	{ enum { type = GL_INT }; enum { components = 1 }; enum { size = sizeof(GLint) }; };

	template<> struct glType<GLuint>
	{ enum { type = GL_UNSIGNED_INT }; enum { components = 1 }; enum { size = sizeof(GLuint) }; };

	template<> struct glType<GLfloat>
	{ enum { type = GL_FLOAT }; enum { components = 1 }; enum { size = sizeof(GLfloat) }; };

	template<> struct glType<GLdouble>
	{ enum { type = GL_DOUBLE }; enum { components = 1 }; enum { size = sizeof(GLdouble) }; };

	template<> struct glType<CVector2<int> >
	{ enum { type = GL_INT }; enum { components = 2 }; enum { size = sizeof(GLint)*2 }; };

	template<> struct glType<CVector2f>
	{ enum { type = GL_FLOAT }; enum { components = 2 }; enum { size = sizeof(GLfloat)*2 }; };

	template<> struct glType<CVector2d>
	{ enum { type = GL_DOUBLE }; enum { components = 2 }; enum { size = sizeof(GLdouble)*2 }; };

	template<> struct glType<CVector3<int> >
	{ enum { type = GL_INT }; enum { components = 3 }; enum { size = sizeof(GLint)*3 }; };

	template<> struct glType<CVector3f>
	{ enum { type = GL_FLOAT }; enum { components = 3 }; enum { size = sizeof(GLfloat)*3 }; };

	template<> struct glType<CVector3d>
	{ enum { type = GL_DOUBLE }; enum { components = 3 }; enum { size = sizeof(GLdouble)*3 }; };

	template<> struct glType<CVector4<int> >
	{ enum { type = GL_INT }; enum { components = 4 }; enum { size = sizeof(GLint)*4 }; };

	template<> struct glType<CVector4f>
	{ enum { type = GL_FLOAT }; enum { components = 4 }; enum { size = sizeof(GLfloat)*4 }; };

	template<> struct glType<CVector4d>
	{ enum { type = GL_DOUBLE }; enum { components = 4 }; enum { size = sizeof(GLdouble)*4 }; };

	template<> struct glType<CMatrix3f>
	{ enum { type = GL_FLOAT }; enum { components = 9 }; enum { size = sizeof(GLfloat)*9 }; };

	template<> struct glType<CMatrix3d>
	{ enum { type = GL_DOUBLE }; enum { components = 9 }; enum { size = sizeof(GLdouble)*9 }; };

	template<> struct glType<CMatrix4f>
	{ enum { type = GL_FLOAT }; enum { components = 16 }; enum { size = sizeof(GLfloat)*16 }; };

	template<> struct glType<CMatrix4d>
	{ enum { type = GL_DOUBLE }; enum { components = 16 }; enum { size = sizeof(GLdouble)*16 }; };
}

#endif
