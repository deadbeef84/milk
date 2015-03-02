#include "milk/renderer/ishader.h"
#include "milk/helper.h"
#include <vector>
#include <fstream>
using namespace milk;
using namespace std;

IShader* IShader::create(const std::string& resourceId)
{
	IShader *pShader = 0;
	if(text::endsWith(resourceId, ".vert"))
		pShader = new CVertexShader;
	else if(text::endsWith(resourceId, ".frag"))
		pShader = new CFragmentShader;

	if(pShader)
		pShader->load(resourceId);
	else
		pShader = new CProgramObject;

	return pShader;
}

void IShader::load(const string& str)
{
	vector<char*> strings;
	ifstream fin(str.c_str());
	if(!fin)
		throw error::file_not_found("Shader file '"+str+"' not found.");
	string line;
	while(getline(fin, line))
	{
		char *pStr = new char[line.length()+2];
		copy(line.begin(), line.end(), pStr);
		pStr[line.length()] = '\n';
		pStr[line.length()+1] = 0;
		strings.push_back(pStr);
	}
	if(!strings.size())
		throw error::opengl("Shader file '"+str+"' is empty?");
	const GLcharARB **ppStr = const_cast<const GLcharARB**>(&strings[0]);
	glShaderSourceARB(m_handle, (GLsizei)strings.size(), ppStr, 0);
	glCompileShaderARB(m_handle);
	delete_range_array(strings.begin(), strings.end());
	glGetObjectParameterivARB(m_handle, GL_OBJECT_COMPILE_STATUS_ARB, &m_status);
	if(!m_status)
		throw error::opengl("Error when loading '" + str + "'\nglCompileShaderARB()\n" + getInfoLog());
}

string IShader::getInfoLog()
{
	GLint infoLogLength;
	glGetObjectParameterivARB(m_handle, GL_OBJECT_INFO_LOG_LENGTH_ARB, &infoLogLength);
	if(infoLogLength)
	{
		GLcharARB *pInfoLog = new GLcharARB[infoLogLength];
		glGetInfoLogARB(m_handle, infoLogLength, NULL, pInfoLog);
		string infoLogStr = pInfoLog;
		delete [] pInfoLog;
		return infoLogStr;
	}
	return "<no info log>";
}
