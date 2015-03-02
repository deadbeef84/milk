#ifndef MILK_ISHADER_H_
#define MILK_ISHADER_H_

#include <string>
#include <fstream>
#include "milk/renderer.h"
#include "milk/glhelper.h"
#include "milk/error.h"
#include "milk/iresource.h"
#include "milk/boost.h"

namespace milk
{
	class IShader : public IResource
	{
	public:
		virtual ~IShader()
		{ }

		MILK_MAKE_RESOURCE(IShader);

		static IShader* create(const std::string& resourceId);

		GLhandleARB getHandle() const
		{ return m_handle; }

		void load(const std::string& str);

		std::string getInfoLog();

	protected:
		GLhandleARB m_handle;
		GLint m_status;
	};

	class CVertexShader : public IShader
	{
	public:
		CVertexShader()
		{
			if(!GLEW_ARB_vertex_shader)
				throw error::milk("The OpenGL extension 'ARB_vertex_shader' is not available.");
			m_handle = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
		}
		virtual ~CVertexShader()
		{
			glDeleteObjectARB(m_handle);
		}

	protected:
	};

	class CFragmentShader : public IShader
	{
	public:
		CFragmentShader()
		{
			if(!GLEW_ARB_fragment_shader)
				throw error::milk("The OpenGL extension 'ARB_fragment_shader' is not available.");
			m_handle = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
		}
		virtual ~CFragmentShader()
		{
			glDeleteObjectARB(m_handle);
		}

	protected:
	};

	class CUniformHandle
	{
	public:
		CUniformHandle(GLint location, GLint components, GLenum type, size_t size)
			: m_location(location), m_components(components), m_type(type), m_size(size)
		{ }

		CUniformHandle(GLint location = -1)
			: m_location(location), m_components(0), m_type(GL_FALSE), m_size(0)
		{ }

		template<class T>
		void setFormat()
		{
			m_components = glType<T>::components;
			m_type = glType<T>::type;
			m_size = glType<T>::size;
		}

		template<class T>
		void set(const T& value) const
		{
			CHECK_FOR_OPENGL_ERRORS("PRIOR");
			BOOST_ASSERT(m_components == glType<T>::components);
			BOOST_ASSERT(m_type == glType<T>::type);
			BOOST_ASSERT(m_size == glType<T>::size);
			glUniform(m_location, value);
			CHECK_FOR_OPENGL_ERRORS("glUniform");
		}

		GLint location() const
		{ return m_location; }

		GLint components() const
		{ return m_components; }

		GLenum type() const
		{ return m_type; }

		size_t size() const
		{ return m_size; }

	protected:
		GLint m_location;
		GLint m_components;
		GLenum m_type;
		size_t m_size;
	};

	class CAttribHandle
	{
	public:
		CAttribHandle(GLint index, GLint components, GLenum type, size_t size)
			: m_index(index), m_components(components), m_type(type), m_size(size)
		{ }

		CAttribHandle(GLint index = -1)
			: m_index(index), m_components(0), m_type(GL_NONE), m_size(0)
		{ }

		/// Compare attribute
		bool operator==(const CAttribHandle& rhs) const
		{
			return m_index == rhs.m_index &&
				   m_components == rhs.m_components &&
				   m_type == rhs.m_type &&
				   m_size == rhs.m_size;
		}

		template<class T>
		void setFormat()
		{
			m_components = glType<T>::components;
			m_type = glType<T>::type;
			m_size = glType<T>::size;
		}

		void vertexPointer(GLsizei stride, const GLvoid *ptr, GLboolean normalized = GL_FALSE) const
		{
			glVertexAttribPointerARB(m_index, m_components, m_type, normalized, stride, ptr);
		}

		void enableVertexArray() const
		{
			glEnableVertexAttribArrayARB(m_index);
		}

		void disableVertexArray() const
		{
			glDisableVertexAttribArrayARB(m_index);
		}

		template<class T>
		void set(const T& value) const
		{
			CHECK_FOR_OPENGL_ERRORS("PRIOR");
			BOOST_ASSERT(m_components == glType<T>::components);
			BOOST_ASSERT(m_type == glType<T>::type);
			BOOST_ASSERT(m_size == glType<T>::size);
			glVertexAttrib(m_index, value);
			CHECK_FOR_OPENGL_ERRORS("glVertexAttrib");
		}

		GLint index() const
		{ return m_index; }

		GLint components() const
		{ return m_components; }

		GLenum type() const
		{ return m_type; }

		size_t size() const
		{ return m_size; }

	protected:
		GLint m_index;
		GLint m_components;
		GLenum m_type;
		size_t m_size;
	};

	class CProgramObject : public IShader
	{
	public:
		CProgramObject()
		{
			if(!GLEW_ARB_shader_objects)
				throw error::milk("The OpenGL extension 'ARB_shader_objects' is not available.");
			m_handle = glCreateProgramObjectARB();
		}

		virtual ~CProgramObject()
		{
			glDeleteObjectARB(m_handle);
		}

		void attach(IShader& shader)
		{
			shader.addRef();
			glAttachObjectARB(m_handle, shader.getHandle());
		}

		void detach(IShader& shader)
		{
			glDetachObjectARB(m_handle, shader.getHandle());
			shader.release();
		}

		template<class T>
		CUniformHandle getUniform(const std::string& name)
		{
			CHECK_FOR_OPENGL_ERRORS("PRIOR");
			GLint uniformLocation = glGetUniformLocationARB(m_handle, name.c_str());
			if(uniformLocation == -1)
				throw error::opengl("glGetUniformLocationARB: Unable to find uniform '"+name+"'.");
			CHECK_FOR_OPENGL_ERRORS("glGetUniformLocationARB");
			return CUniformHandle(uniformLocation, glType<T>::components, glType<T>::type, glType<T>::size);
		}

		template<class T>
		CAttribHandle getAttrib(const std::string& name)
		{
			CHECK_FOR_OPENGL_ERRORS("PRIOR");
			GLint attribLocation = glGetAttribLocationARB(m_handle, name.c_str());
			if(attribLocation == -1)
				throw error::opengl("glGetAttribLocationARB: Unable to find attribute '"+name+"'.");
			CHECK_FOR_OPENGL_ERRORS("glGetAttribLocationARB");
			return CAttribHandle(attribLocation, glType<T>::components, glType<T>::type, glType<T>::size);
		}

		void bind()
		{
			glUseProgramObjectARB(m_handle);
		}

		static void unbind()
		{
			glUseProgramObjectARB(0);
		}

		void link()
		{
			glLinkProgramARB(m_handle);
			glGetObjectParameterivARB(m_handle, GL_OBJECT_LINK_STATUS_ARB, &m_status);

			if(!m_status)
				throw error::opengl("glLinkProgramARB()\n"+getInfoLog());

			// FIXME: write warning?
			//if(getInfoLog().find("software") != std::string::npos)
				//;
		}
	};
}

#endif
