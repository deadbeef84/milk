#include "milk/renderer/cmodel_ms3d.h"
#ifdef AOEUHASOTEHU
#include "milk/cimage.h"
#include <map>
using namespace milk;
using namespace std;

//////////////////////////////////////////

#include "milk/pack8enable.h"

// File header
struct MS3DHeader
{
	char m_ID[10];
	int m_version;
} MILK_PACK_STRUCT;

struct MS3DMesh
{
	char m_materialIndex;
	vectorPOD<ushort> m_triangleIndices;
} MILK_PACK_STRUCT;

// Vertex information
struct MS3DVertex
{
	uchar m_flags;
	float m_vertex[3];
	char m_boneID;
	uchar m_refCount;
} MILK_PACK_STRUCT;

// Triangle information
struct MS3DTriangle
{
	ushort m_flags;
	ushort m_vertexIndices[3];
	float m_vertexNormals[3][3];
	float m_s[3], m_t[3];
	uchar m_smoothingGroup;
	uchar m_groupIndex;
} MILK_PACK_STRUCT;

// Material information
struct MS3DMaterial
{
	char m_name[32];
	CColor4f m_ambient, m_diffuse, m_specular, m_emissive;
	float m_shininess;	// 0.0f - 128.0f
	float m_transparency;	// 0.0f - 1.0f
	uchar m_mode;	// 0, 1, 2 is unused now
	char m_texture[128];
	char m_alphamap[128];
} MILK_PACK_STRUCT;

//	Joint information
struct MS3DJoint
{
	uchar m_flags;
	char m_name[32];
	char m_parentName[32];
	float m_rotation[3];
	float m_translation[3];
	ushort m_numRotationKeyframes;
	ushort m_numTranslationKeyframes;
} MILK_PACK_STRUCT;

// Keyframe data
struct MS3DKeyframe
{
	float m_time;
	float m_parameter[3];
} MILK_PACK_STRUCT;

#include "milk/pack8disable.h"

static const float eps = math::sqr(0.0000001f);

struct StripVertex
{
	StripVertex()
		: bone(-2)
	{
	}

	StripVertex(const float *pVert, const float *pNorm, float tu, float tv, char bone)
		: v(pVert), n(pNorm), uv(tu, tv), bone(bone)
	{ }

	CVector3f v;
	CVector3f n;
	CVector2f uv;
	char bone;

#define ABS(x) (x>0?x:-x)
//#define EQ(a, b) ((ABS(a.x - b.x) + ABS(a.y - b.y) + ABS(a.z - b.z)) < eps)
//#define EQ(a, b) ((a.x == b.x) && (a.y == b.y) && (a.z == b.z))

	operator==(const StripVertex& rhs)
	{
		if(bone == rhs.bone)
			//if(EQ(v, rhs.v))
			if(abs2(v-rhs.v) < eps)
				if(abs2(uv-rhs.uv) < eps)
					if(abs2(n-rhs.n) < eps)
						return true;
		return false;
	}
};

struct StripTriangle
{
	int indices[3];
	vectorPOD<StripTriangle*> connections;
};

//////////////////////////////////////////

template<class T>
static T readFromData(const uchar *&pData)
{
	T tmp = *(T*)pData;
	pData += sizeof(T);
	return tmp;
}

template<class T>
static T* getDataPtr(const uchar *&pData)
{
	T* ret = (T*)pData;
	pData += sizeof(T);
	return ret;
}

bool CModelImporterMS3D::canImport(std::string filename)
{
	FILE *inputFile = fopen(filename.c_str(), "rb");
	if(!inputFile)
		return false;

	MS3DHeader header;
	fread(&header, sizeof(header), 1, inputFile);
	fclose(inputFile);

	if(strncmp(header.m_ID, "MS3D000000", 10) != 0)
		return false;

	if(header.m_version < 3)
		return false;

	return true;
}

bool CModelImporterMS3D::load(std::string filename)
{
	FILE *inputFile = fopen(filename.c_str(), "rb");
	if(!inputFile)
		throw error::file_not_found("Could not find file \""+filename+"\".");

	string basepath;
	string::size_type pos = filename.rfind('/');
	if(pos != string::npos)
		basepath = filename.substr(0, pos + 1);
	else
		basepath = "";

	fseek(inputFile, 0, SEEK_END);
	long fileSize = ftell(inputFile);
	fseek(inputFile, 0, SEEK_SET);

	uchar *pBuffer = new uchar[fileSize];
	fread(pBuffer, fileSize, 1, inputFile);
	fclose(inputFile);
	const uchar *pPtr = pBuffer;

	////////////////////////////////////////////////////////////////

	MS3DHeader *pHeader = getDataPtr<MS3DHeader>(pPtr);

	if(strncmp(pHeader->m_ID, "MS3D000000", 10) != 0)
		throw error::milk("Model format error, header not recognized.");

	if(pHeader->m_version < 3)
		throw error::milk("Model format error, MilkShape3D version not supported.");

	////////////////////////////////////////////////////////////////

	// Load vertices
	int nVertices = readFromData<ushort>(pPtr);
	vector<MS3DVertex> vertices((MS3DVertex*)pPtr, (MS3DVertex*)pPtr + nVertices);
	pPtr += sizeof(MS3DVertex) * nVertices;

	// Load triangles
	int nTriangles = readFromData<ushort>(pPtr);
	vector<MS3DTriangle> triangles((MS3DTriangle*)pPtr, (MS3DTriangle*)pPtr + nTriangles);
	pPtr += sizeof(MS3DTriangle) * nTriangles;

	// Load groupes, ie meshes
	int nGroups = readFromData<ushort>(pPtr);
	vector<MS3DMesh> meshes(nGroups);
	for(int i = 0; i < nGroups; i++)
	{
		pPtr += sizeof(uchar);	// flags
		pPtr += 32;				// name
		ushort nTriIndices = readFromData<ushort>(pPtr);
		meshes[i].m_triangleIndices.resize(nTriIndices);
		for(int j = 0; j < nTriIndices; j++)
			meshes[i].m_triangleIndices[j] = readFromData<ushort>(pPtr);
		meshes[i].m_materialIndex = readFromData<char>(pPtr);
	}

	// Load materials
	int nMaterials = readFromData<ushort>(pPtr);
	vector<MS3DMaterial> materials((MS3DMaterial*)pPtr, (MS3DMaterial*)pPtr + nMaterials);
	model.materials.resize(nMaterials);
	for(int i = 0; i < nMaterials; i++)
	{
		model.materials[i] = new CMaterial;
		string texfn = materials[i].m_texture;
		if(!texfn.empty())
		{
			string rel = texfn.substr(0, 2);
			if(rel == ".\\" || rel == "./")
				// MS3D 1.5.x relative path
				texfn = basepath + texfn.substr(2);
			//else
				// MS3D 1.4.x or earlier - absolute path
			CImage img(texfn);
			img.scaleToTexture();
			model.materials[i]->setTexture(0, new CTexture2D(img));
		}
		model.materials[i]->setAmbient(materials[i].m_ambient);
		model.materials[i]->setDiffuse(materials[i].m_diffuse);
		model.materials[i]->setSpecular(materials[i].m_specular);
		model.materials[i]->setEmissive(materials[i].m_emissive);
		model.materials[i]->setShininess(materials[i].m_shininess);
		//model.materials[i]->m_mode = materials[i].m_mode;
	}

	// Load Skeletal Animation Stuff
	float m_animfps = readFromData<float>(pPtr);
	pPtr += sizeof(float); // skip currentTime
	int totalFrames = readFromData<long>(pPtr);

	// Joint data
	int nJoints = readFromData<ushort>(pPtr);
	//model.joints.resize(nJoints);

	const uchar *pTempPtr = pPtr;

	map<string, int> jointNameList;
	for(int i = 0; i < nJoints; i++)
	{
		MS3DJoint *pJoint = getDataPtr<MS3DJoint>(pTempPtr);
		pTempPtr += sizeof(MS3DKeyframe) * (pJoint->m_numRotationKeyframes+pJoint->m_numTranslationKeyframes);
		jointNameList[text::strToLower(pJoint->m_name)] = i;
	}

	for(int i = 0; i < nJoints; i++)
	{
		MS3DJoint *pJoint = getDataPtr<MS3DJoint>(pTempPtr);

		/*
		int parentIndex = -1;
		if(strlen(pJoint->m_parentName) > 0)
		{
			map<string, int>::iterator it = jointNameList.find(text::strToLower(pJoint->m_parentName));
			if(it == jointNameList.end())
			{
				// Unable to find parent bone in MS3D file
				delete[] pBuffer;
				return false;
			}
			else
				parentIndex = it->second;
		}

		model.joints[i].m_matRelative = matrixTransform(CVector3f(pJoint->m_translation), CVector3f(pJoint->m_rotation));
		model.joints[i].m_name = pJoint->m_name;
		model.joints[i].m_parent = parentIndex;
		model.joints[i].m_rotationKeyframes.resize(pJoint->m_numRotationKeyframes);
		model.joints[i].m_translationKeyframes.resize(pJoint->m_numTranslationKeyframes);

		for(int j = 0; j < pJoint->m_numRotationKeyframes; j++)
		{
			MS3DKeyframe *pKeyframe = getDataPtr<MS3DKeyframe>(pPtr);
			model.joints[i].m_rotationKeyframes[j].m_jointIndex = i;
			model.joints[i].m_rotationKeyframes[j].m_time = pKeyframe->m_time;
			model.joints[i].m_rotationKeyframes[j].m_parameter = pKeyframe->m_parameter;
			std::swap(model.joints[i].m_rotationKeyframes[j].m_parameter.x, model.joints[i].m_rotationKeyframes[j].m_parameter.z);
		}

		for(int j = 0; j < pJoint->m_numTranslationKeyframes; j++)
		{
			MS3DKeyframe *pKeyframe = getDataPtr<MS3DKeyframe>(pPtr);
			model.joints[i].m_translationKeyframes[j].m_jointIndex = i;
			model.joints[i].m_translationKeyframes[j].m_time = pKeyframe->m_time;
			model.joints[i].m_translationKeyframes[j].m_parameter = pKeyframe->m_parameter;
		}
		*/
	}
	delete[] pBuffer;

	/////////////////////
	// Setup Joints

	/*
	// Calculate relative and absolute matrices for every joint
	for(vector<CJoint>::iterator it = model.joints.begin(); it != model.joints.end(); it++)
	{
		if(it->m_parent != -1)
			it->m_matAbsolute = model.joints[it->m_parent].m_matAbsolute * it->m_matRelative;
		else
			it->m_matAbsolute = it->m_matRelative;
	}
	*/

	/*
	// Multiply every vertex by the inverse of its joint's absolute-matrix
	for(vectorPOD<SVertex>::iterator it = m_vertices.begin(); it != m_vertices.end(); it++)
	{
		if(it->m_boneID != -1)
		{
			CMatrix4f matrix = inverse(model.joints[it->m_boneID].m_matAbsolute);
			it->m_location = matrix * CVector4f(it->m_location, 1.0f);
		}
	}

	// Do the same with the normals as with the vertices (almost)
	for(vectorPOD<STriangle>::iterator it = m_triangles.begin(); it != m_triangles.end(); it++)
	{
		for(int j = 0; j < 3; j++)
		{
			const SVertex& vertex = m_vertices[it->m_vertexIndices[j]];
			if(vertex.m_boneID != -1)
			{
				// Get the inverse of the absolute matrix
				CMatrix4f matrix = inverse(model.joints[vertex.m_boneID].m_matAbsolute);
				// Multiply the normal with the matrix (w=0.0 because we are transforming a normal)
				it->m_vertexNormals[j] = matrix * CVector4f(it->m_vertexNormals[j], 0.0f);
			}
		}
	}
	*/



	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	model.meshes.resize(meshes.size());
	int mi = 0;
	for(vector<MS3DMesh>::const_iterator itm = meshes.begin(); itm != meshes.end(); ++itm, ++mi)
	{
		CMesh& mesh = model.meshes[mi];
		mesh.m_materialIndex = itm->m_materialIndex;
		mesh.m_renderMode = GL_TRIANGLES;

		mesh.m_pVertexBuffer = IVertexBuffer::create(CVertexFormat(3,3,2,0), uint(itm->m_triangleIndices.size() * 3));
		mesh.m_pVertexBuffer->lock();

		CDataContainer<CVector3f>::iterator vit = mesh.m_pVertexBuffer->getVertices3f().begin();
		CDataContainer<CVector3f>::iterator nit = mesh.m_pVertexBuffer->getNormals3f().begin();
		CDataContainer<CVector2f>::iterator tit = mesh.m_pVertexBuffer->getTexCoords2f(0).begin();
		
		for(vectorPOD<ushort>::const_iterator itt = itm->m_triangleIndices.begin(); itt != itm->m_triangleIndices.end(); ++itt)
		{
			const MS3DTriangle& tri = triangles[*itt];
			for(int k = 0; k < 3; ++k)
			{
				MS3DVertex& vertex = vertices[tri.m_vertexIndices[k]];
				*vit++ = vertex.m_vertex;
				*nit++ = tri.m_vertexNormals[k];
				*tit++ = CVector2f(tri.m_s[k], tri.m_t[k]);
				//vertex.m_boneID
			}
		}
		BOOST_ASSERT(vit == mesh.m_pVertexBuffer->getVertices3f().end());
		mesh.m_pVertexBuffer->unlock();
	}

	return true;
}

#endif