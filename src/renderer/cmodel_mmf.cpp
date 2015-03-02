#include "milk/renderer/cmodel_mmf.h"
#include "milk/cimage.h"
#include "milk/renderer/ivertexbuffernormal.h"
#include "milk/scenegraph/ccamera.h"
#include <limits>
using namespace milk;
using namespace milk::milkModelFile;
using namespace std;

//////////////////////////////////////////

#include "milk/pack8enable.h"
struct MMFHeader
{
	char identifier[3];
	ushort version;
	ulong numMaterials;
	ulong numBones;
	ulong numAnimations;
	ulong numMeshes;
	ulong numFrames;
	float frameRate;
	char reserved[35];
} MILK_PACK_STRUCT;
#include "milk/pack8disable.h"

struct Wee
{
	bool operator()(const weight& a, const weight& b)
	{
		return a.first > b.first;
	}
};

//////////////////////////////////////////

template<class T>
static T freadtype(FILE *fp)
{
	T tmp;
	fread(&tmp, sizeof(T), 1, fp);
	return tmp;
}
template<>
static std::string freadtype(FILE *fp)
{
	ulong len = freadtype<ulong>(fp);
	std::string ret;
	ret.resize(len);
	fread(&ret[0], len, 1, fp);
	return ret;
}
template<>
static CVector3f freadtype(FILE *fp)
{
	CVector3f tmp;
	fread(&tmp, sizeof(float)*3, 1, fp);
	return tmp;
}
template<>
static CColor4f freadtype(FILE *fp)
{
	CColor4f tmp;
	fread(&tmp, sizeof(float)*4, 1, fp);
	return tmp;
}

template<>
static CMatrix4f freadtype(FILE *fp)
{
	CMatrix4f tmp;
	fread(&tmp, sizeof(float)*16, 1, fp);
	return tmp;
}

template<class T>
static void freadvector(FILE *fp, vector<T>& v, uint num)
{
	v.resize(num);
	fread(&v[0], sizeof(T)*num, 1, fp);
}

//////////////////////////////////////////////////////////////////////////

float CComponent::getValue(float frame) const
{
	if(!m_keyframes.size())
		return 0.0f;

	keyframeList::const_iterator n = m_keyframes.lower_bound(frame);
	if(n == m_keyframes.begin())
		return n->second.w;
	else if(n == m_keyframes.end())
	{
		const CVector4f& v = (--n)->second;
		return v.x + v.y + v.z + v.w;
	}
	else
	{
		keyframeList::const_iterator p = n;
		--p;
		float t = math::clamp((frame - p->first) / (n->first - p->first), 0.0f, 1.0f);
		return t * (t * (t * p->second.x + p->second.y) + p->second.z) + p->second.w;
	}
}

//////////////////////////////////////////////////////////////////////////

CMatrix4f CBoneSource::getBoneMatrix(float frame)
{
	// Get the value of every component...
	float data[6];
	for(int c = 0; c < 6; ++c)
	{
		if(c < 3)
			data[c] = m_components[c].m_keyframes.size() ? m_components[c].getValue(frame) : m_translation[c];
		else
			data[c] = m_components[c].getValue(frame);
	}

	return CMatrix4f(matrixRotation3(m_rotation) * matrixRotation3(data[3], data[4], data[5]) * matrixRotation3(m_postRotation), CVector3f(data));
}

CMatrix4f CBoneSource::getBoneMatrixPose()
{
	return CMatrix4f(matrixRotation3(m_rotation) * matrixRotation3(m_postRotation), m_translation);
}

//////////////////////////////////////////////////////////////////////////

bool CModel::load(string filename)
{
	unload();

	FILE *inputFile = fopen(filename.c_str(), "rb");
	if(!inputFile)
		throw error::file_not_found("Could not find file \""+filename+"\".");

	string::size_type pos = filename.rfind('/');
	if(pos == string::npos)
	pos = filename.rfind('\\');
	string basepath = (pos != string::npos) ? filename.substr(0, pos + 1) : string("");

	////////////////////////////////////////////////////////////////

	if(sizeof(MMFHeader) != 64)
		throw error::milk("Internal error, header size != 64.");

	MMFHeader header = freadtype<MMFHeader>(inputFile);

	if(strncmp(header.identifier, "MMF", 3) != 0)
		throw error::milk("Model format error, header not recognized.");

	if(header.version > 2)
		throw error::milk("Model format error, version not supported.");

	////////////////////////////////////////////////////////////////

	m_materials.resize(header.numMaterials);
	for(uint i = 0; i < header.numMaterials; i++)
	{
		if(header.numBones)
		{
			CAppearance *pMaterial = new CAppearance();
			pMaterial->getPass(0).loadShader("data/shaders/skinning.vert", "data/shaders/skinning.frag");
			m_materials[i] = pMaterial;
		}
		else
		{
			CAppearance *pMaterial = new CAppearance;
			pMaterial->getPass(0).loadShader("data/shaders/test.vert", "data/shaders/test.frag");
			m_materials[i] = pMaterial;
		}
		//else
		//	m_materials[i] = new CMaterial;

		string materialName = freadtype<string>(inputFile);
		//pMaterial->setName(materialName);
		freadtype<ulong>(inputFile);
		//pMaterial->setType(mt);
		//pMaterial->setType(MATERIALTYPE_GLSL);

		Material mat;
		mat.setAmbient(freadtype<CColor4f>(inputFile));
		mat.setDiffuse(freadtype<CColor4f>(inputFile));
		mat.setSpecular(freadtype<CColor4f>(inputFile));
		mat.setEmissive(freadtype<CColor4f>(inputFile));
		mat.setShininess(freadtype<float>(inputFile));

		m_materials[i]->getPass(0).setMaterial(mat);

		ulong numTextures = freadtype<ulong>(inputFile);
		for(uint t = 0; t < numTextures; t++)
		{
			string textureFilename = freadtype<string>(inputFile);
			if(!textureFilename.empty())
			{
				string rel = textureFilename.substr(0, 2);
				if(rel == ".\\" || rel == "./") // relative path
					textureFilename = basepath + textureFilename.substr(2);
				m_materials[i]->getPass(0).setTexture(t, getResource<ITexture>(textureFilename));
			}

			CProgramObject& po = *m_materials[i]->getPass(0).getProgramObject();
			po.bind();
			try
			{
				if(textureFilename.find("normal") != string::npos)
					po.getUniform<GLint>("normalMap").set(GLint(t));
				else if(textureFilename.find("shin") != string::npos)
					po.getUniform<GLint>("shininessMap").set(GLint(t));
				else if(textureFilename.find("bump") != string::npos)
					po.getUniform<GLint>("bumpMap").set(GLint(t));
				else// if(textureFilename.find("color") != string::npos)
					po.getUniform<GLint>("baseMap").set(GLint(t));
			}
			catch(...) { }
			po.unbind();
		}

		/*
		// shadow texture
		CAppearance *pMat = m_materials[i];
		if(pMat)
		{
			//m_materials[i]->setTexture(numTextures, CShadowTexture::get());
			CProgramObject& po = *pMat->getPass(0).getProgramObject();
			po.bind();
			try
			{
				po.getUniform<GLint>("baseMap").set(GLint(0));
				po.getUniform<GLint>("normalMap").set(GLint(1));
				po.getUniform<GLint>("shininessMap").set(GLint(2));
				po.getUniform<GLint>("shadowMap").set(GLint(numTextures));
			}
			catch(...) { }
			po.unbind();
		}
		*/
	}

	float minFrame = numeric_limits<float>::max();
	float maxFrame = numeric_limits<float>::min();
	m_boneSources.resize(header.numBones);
	for(uint i = 0; i < header.numBones; ++i)
	{
		CBoneSource *pBoneSource = new CBoneSource;
		m_boneSources[i] = pBoneSource;

		pBoneSource->m_name = freadtype<string>(inputFile);
		pBoneSource->m_parent = freadtype<long>(inputFile);

		pBoneSource->m_translation = freadtype<CVector3f>(inputFile);
		pBoneSource->m_rotation = freadtype<CVector3f>(inputFile);
		pBoneSource->m_postRotation = freadtype<CVector3f>(inputFile);
		pBoneSource->m_worldInv = freadtype<CMatrix4f>(inputFile);

		for(int c = 0; c < 6; ++c)
		{
			ulong numKeyframes = freadtype<ulong>(inputFile);
			uchar interp = freadtype<uchar>(inputFile); // FIXME: remove this?
			for(uint k = 0; k < numKeyframes; ++k)
			{
				float frame = freadtype<float>(inputFile);
				if(interp == 0)
				{
					/*
					keyframe.m_frame = freadtype<float>(inputFile);
					keyframe.m_data[0] = freadtype<float>(inputFile);
					*/
				}
				else
				{
					pBoneSource->m_components[c].m_keyframes.insert(
						make_pair(frame, freadtype<CVector4f>(inputFile)) );
				}
				minFrame = min(frame, minFrame);
				maxFrame = max(frame, maxFrame);
			}
		}
	}

	m_animations.push_back(CAnimation(minFrame, maxFrame, 25.0f));

	m_meshes.resize(header.numMeshes);
	for(uint i = 0; i < header.numMeshes; i++)
	{
		uchar lists = 0;
		CMesh& mesh = m_meshes[i];

		// Load vertices
		if(header.version == 1)
			lists = freadtype<uchar>(inputFile);
		mesh.m_materialIndex = freadtype<long>(inputFile);
		uint usedBones = freadtype<ulong>(inputFile);
		uint numVertices = freadtype<ulong>(inputFile);

		vector<CSkinnedVertex>& vertices = mesh.m_skinnedVertices;
		vertices.resize(numVertices);

		CAppearance *pMat = m_materials[mesh.m_materialIndex];
		bool skinning = pMat && header.numBones;

		int skinIndicesAttrib = -1;
		int skinWeightsAttrib = -1;
		int tangentAttrib = -1;
		int bitangentAttrib = -1;

		CVertexFormat vf(3,3,2);
		if(skinning)
		{
			CProgramObject& po = *pMat->getPass(0).getProgramObject();

			for(uint i = 0; i < header.numBones; ++i)
				m_boneSources[i]->setSkinMatrixUniform(po.getUniform<CMatrix4f>("skinMatrices["+toStr(i)+"]"));

			mesh.m_hwSkinning = true;
			skinIndicesAttrib = vf.addShaderAttrib(po.getAttrib<CVector3f>("skinIndices"));
			skinWeightsAttrib = vf.addShaderAttrib(po.getAttrib<CVector2f>("skinWeights"));
		}

		bool tangent = true && pMat;

		if(tangent)
		{
			CProgramObject& po = *pMat->getPass(0).getProgramObject();
			try
			{
				tangentAttrib = vf.addShaderAttrib(po.getAttrib<CVector3f>("tangent"));
				bitangentAttrib = vf.addShaderAttrib(po.getAttrib<CVector3f>("bitangent"));
			}
			catch(error::milk e)
			{
			}
		}

		mesh.m_pVertexBuffer = IVertexBuffer::create(vf, numVertices);
		//mesh.m_pVertexBuffer = new CVertexBufferNormal_GL(vf, numVertices, DYNAMIC);
		mesh.m_pVertexBuffer->lock();
		{
			vector<CSkinnedVertex>::iterator it = vertices.begin();
			CDataContainer<CVector3f>::iterator vit = mesh.m_pVertexBuffer->getVertices3f().begin();
			CDataContainer<CVector3f>::iterator nit = mesh.m_pVertexBuffer->getNormals3f().begin();
			CDataContainer<CVector2f>::iterator tit = mesh.m_pVertexBuffer->getTexCoords2f(0).begin();
			for(; it != vertices.end(); ++it, ++vit, ++nit, ++tit)
			{
				// read from file
				it->p = freadtype<CVector3f>(inputFile);
				it->n = freadtype<CVector3f>(inputFile);
				CVector2f uv = freadtype<CVector2f>(inputFile);
				uchar numWeights = freadtype<uchar>(inputFile);
				for(int i = 0; i < numWeights; ++i)
				{
					char index = freadtype<char>(inputFile);
					float w = freadtype<float>(inputFile);
					it->weights.push_back(make_pair<int,float>(index,w));
				}
				// put into vertexbuffer...
				*vit = it->p;
				*nit = it->n;
				*tit = uv;
			}
			BOOST_ASSERT(vit == mesh.m_pVertexBuffer->getVertices3f().end());

			if(skinning)
			{
				it = vertices.begin();
				CDataContainer<CVector3f>::iterator siit = mesh.m_pVertexBuffer->getAttrib3f(skinIndicesAttrib).begin();
				CDataContainer<CVector2f>::iterator swit = mesh.m_pVertexBuffer->getAttrib2f(skinWeightsAttrib).begin();
				for(; it != vertices.end(); ++it, ++siit, ++swit)
				{
					int numWeights = it->weights.size();
					if(numWeights == 0)
					{
						// FIXME: hmm?
						siit->set(0.0f, 0.0f, 0.0f);
						swit->set(1.0f, 0.0f);
					}
					else if(numWeights == 1)
					{
						siit->set(float(it->weights[0].first)+0.5f, 0.0f, 0.0f);
						swit->set(it->weights[0].second, 0.0f);
					}
					else if(numWeights == 2)
					{
						siit->set(float(it->weights[0].first)+0.5f, float(it->weights[1].first)+0.5f, 0.0f);
						swit->set(it->weights[0].second, it->weights[1].second);
					}
					else if(numWeights == 3)
					{
						siit->set(float(it->weights[0].first)+0.5f, float(it->weights[1].first)+0.5f, float(it->weights[2].first)+0.5f);
						swit->set(it->weights[0].second, it->weights[1].second);
					}
					else
					{
						sort(it->weights.begin(), it->weights.end(), Wee());
						CVector3f v(it->weights[0].second, it->weights[1].second, it->weights[2].second);
						v.normalize();
						siit->set(float(it->weights[0].first)+0.5f, float(it->weights[1].first)+0.5f, float(it->weights[2].first)+0.5f);
						swit->set(v.x, v.y);
					}
				}
			}
			mesh.m_pVertexBuffer->unlock();
		}

		// Load indices
		if(header.version == 2)
			lists = freadtype<uchar>(inputFile);
		uint indexFormat = freadtype<ulong>(inputFile);
		uint numIndices = freadtype<ulong>(inputFile);

		mesh.m_renderMode = lists ? GL_TRIANGLES : GL_TRIANGLE_STRIP;

		if(numIndices)
		{
			vector<ushort> indices;
			freadvector<ushort>(inputFile, indices, numIndices);

			mesh.m_pIndexBuffer = IIndexBuffer::create(GL_UNSIGNED_SHORT, numIndices);
			mesh.m_pIndexBuffer->lock();
			{
				vector<ushort>::const_iterator it = indices.begin();
				ushort* iit = mesh.m_pIndexBuffer->getIndicesus();
				for(; it != indices.end(); ++it, ++iit)
					*iit = *it;
			}
			mesh.m_pIndexBuffer->unlock();
		}

		if(tangent && tangentAttrib != -1 && bitangentAttrib != -1)
		{
			/**/
			mesh.m_pVertexBuffer->lock();
			mesh.m_pIndexBuffer->lock(READ);

			vector<CVector3f> tan1(mesh.m_pVertexBuffer->numVertices(), normalize(CVector3f(1.0f, 1.0f, 1.0f)));
			vector<CVector3f> tan2(mesh.m_pVertexBuffer->numVertices(), normalize(CVector3f(1.0f, 1.0f, 1.0f)));

			CDataContainer<CVector3f>::iterator vit = mesh.m_pVertexBuffer->getVertices3f().begin();
			CDataContainer<CVector2f>::iterator tit = mesh.m_pVertexBuffer->getTexCoords2f(0).begin();

			ushort* iit = mesh.m_pIndexBuffer->getIndicesus() + 2;
			uint step = mesh.m_renderMode == GL_TRIANGLES ? 3 : 1;
			for(uint i = 2; i < mesh.m_pIndexBuffer->numIndices(); iit+=step, i+=step)
			{
				uint i0 = *(iit-2), i1 = *(iit-1), i2 = *iit;

				// degenerate?
				if(i0 == i1 || i0 == i2 || i1 == i2)
					continue;

				const CVector3f& v1 = vit[i0];
				const CVector3f& v2 = vit[i1];
				const CVector3f& v3 = vit[i2];
				const CVector2f& w1 = tit[i0];
				const CVector2f& w2 = tit[i1];
				const CVector2f& w3 = tit[i2];

				float x1 = v2.x - v1.x;
				float x2 = v3.x - v1.x;
				float y1 = v2.y - v1.y;
				float y2 = v3.y - v1.y;
				float z1 = v2.z - v1.z;
				float z2 = v3.z - v1.z;

				float s1 = w2.x - w1.x;
				float s2 = w3.x - w1.x;
				float t1 = w2.y - w1.y;
				float t2 = w3.y - w1.y;

				float r = 1.0f / (s1 * t2 - s2 * t1);
				CVector3f sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
				CVector3f tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);

				tan1.at(i0) += sdir;
				tan1.at(i1) += sdir;
				tan1.at(i2) += sdir;

				tan2.at(i0) += tdir;
				tan2.at(i1) += tdir;
				tan2.at(i2) += tdir;
			}
			mesh.m_pIndexBuffer->unlock();

			CDataContainer<CVector3f>::iterator nit = mesh.m_pVertexBuffer->getNormals3f().begin();
			CDataContainer<CVector3f>::iterator tait = mesh.m_pVertexBuffer->getAttrib3f(tangentAttrib).begin();
			CDataContainer<CVector3f>::iterator btit = mesh.m_pVertexBuffer->getAttrib3f(bitangentAttrib).begin();
			for(uint i = 0; i < mesh.m_pVertexBuffer->numVertices(); ++i, ++tait, ++btit, ++nit)
			{
				CVector3f n = *nit;
				CVector3f t = tan1[i];

				// Gram-Schmidt orthogonalize
				CVector3f tangent = normalize( t - n * dot(n, t) );
				CVector3f bitangent = cross(n, tangent);

				*tait = tangent;
				*btit = bitangent;

				// Calculate handedness
				//tangent[i].w = (dot(cross(n, t), tan2[i]) < 0.0F) ? -1.0F : 1.0F;
			}
			BOOST_ASSERT(tait == mesh.m_pVertexBuffer->getAttrib3f(tangentAttrib).end());
			mesh.m_pVertexBuffer->unlock();
			/*/
			IVertexBuffer *pVB = IVertexBuffer::create(mesh.m_pVertexBuffer->format(), (mesh.m_pIndexBuffer->numIndices()-2)*3);
			pVB->lock();

			mesh.m_pVertexBuffer->lock();
			mesh.m_pIndexBuffer->lock(READ);

			CDataContainer<CVector3f>::iterator nv = pVB->getVertices3f().begin();
			CDataContainer<CVector2f>::iterator nt = pVB->getTexCoords2f(0).begin();
			CDataContainer<CVector3f>::iterator nn = pVB->getNormals3f().begin();
			CDataContainer<CVector3f>::iterator nta = pVB->getAttrib3f(tangentAttrib).begin();
			CDataContainer<CVector3f>::iterator nbi = pVB->getAttrib3f(bitangentAttrib).begin();

			CDataContainer<CVector3f>::iterator vit = mesh.m_pVertexBuffer->getVertices3f().begin();
			CDataContainer<CVector2f>::iterator tit = mesh.m_pVertexBuffer->getTexCoords2f(0).begin();

			ushort* iit = mesh.m_pIndexBuffer->getIndicesus() + 2;
			uint step = mesh.m_renderMode == GL_TRIANGLES ? 3 : 1;
			for(uint i = 2; i < mesh.m_pIndexBuffer->numIndices(); iit+=step, i+=step)
			{
				uint i0 = *(iit-2), i1 = *(iit-1), i2 = *iit;

				// degenerate?
				if(i0 == i1 || i0 == i2 || i1 == i2)
					continue;

				const CVector3f& v1 = vit[i0];
				const CVector3f& v2 = vit[i1];
				const CVector3f& v3 = vit[i2];
				const CVector2f& w1 = tit[i0];
				const CVector2f& w2 = tit[i1];
				const CVector2f& w3 = tit[i2];

				float x1 = v2.x - v1.x;
				float x2 = v3.x - v1.x;
				float y1 = v2.y - v1.y;
				float y2 = v3.y - v1.y;
				float z1 = v2.z - v1.z;
				float z2 = v3.z - v1.z;

				float s1 = w2.x - w1.x;
				float s2 = w3.x - w1.x;
				float t1 = w2.y - w1.y;
				float t2 = w3.y - w1.y;

				float r = 1.0f / (s1 * t2 - s2 * t1);
				CVector3f sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
				CVector3f tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);

				CVector3f n = normalize(cross(v2-v1, v3-v1));
				CVector3f t = normalize(tdir - n * dot(n, tdir) );
				CVector3f b = cross(n, t);

				*nv++ = v1;
				*nt++ = w1;
				*nn++ = n;
				*nta++ = t;
				*nbi++ = b;

				*nv++ = v2;
				*nt++ = w2;
				*nn++ = n;
				*nta++ = t;
				*nbi++ = b;

				*nv++ = v3;
				*nt++ = w3;
				*nn++ = n;
				*nta++ = t;
				*nbi++ = b;
			}

			mesh.m_pIndexBuffer->unlock();
			mesh.m_pVertexBuffer->unlock();

			pVB->unlock();

			safeDelete(mesh.m_pIndexBuffer);
			safeDelete(mesh.m_pVertexBuffer);
			mesh.m_pVertexBuffer = pVB;
			/**/
		}

		/*
		mesh.m_pVertexBuffer->lock();
		{
			CDataContainer<CVector3f>::iterator tait = mesh.m_pVertexBuffer->getAttrib3f(tangentAttrib).begin();
			CDataContainer<CVector3f>::iterator btit = mesh.m_pVertexBuffer->getAttrib3f(bitangentAttrib).begin();
			for(int i = 0; i < mesh.m_pVertexBuffer->numVertices(); ++i, ++tait, ++btit)
			{
				CVector3f tan = *tait;
				CVector3f bitan = *btit;

				float l = tait->abs2();

				if(l < 0.9f || l > 1.1f)
					l = 1.0f;

				l = btit->abs2();

				if(l < 0.9f || l > 1.1f)
					l = 1.0f;

				if(dot(tan, bitan) > 0.1f || dot(tan, bitan) < -0.1f)
					l = 1.0f;
			}
			BOOST_ASSERT(tait == mesh.m_pVertexBuffer->getAttrib3f(tangentAttrib).end());
		}
		mesh.m_pVertexBuffer->unlock();
		*/
	}

	fclose(inputFile);

	return true;
}


/*
struct Triangle
{
	unsigned short	index[3];
};


void CalculateTangentArray(const Point3D *vertex, const Vector3D *normal, const Point2D *texcoord,
						   long triangleCount, const Triangle *triangle, Vector4D *tangent)
{
	Vector3D *tan1 = new Vector3D[vertexCount * 2];
	Vector3D *tan2 = tan1 + vertexCount;
	ClearMemory(tan1, vertexCount * sizeof(Vector3D) * 2);

	for (long a = 0; a < triangleCount; a++)
	{
		long i1 = triangle->index[0];
		long i2 = triangle->index[1];
		long i3 = triangle->index[2];

		const Point3D& v1 = vertex[i1];
		const Point3D& v2 = vertex[i2];
		const Point3D& v3 = vertex[i3];

		const Point2D& w1 = texcoord[i1];
		const Point2D& w2 = texcoord[i2];
		const Point2D& w3 = texcoord[i3];

		float x1 = v2.x - v1.x;
		float x2 = v3.x - v1.x;
		float y1 = v2.y - v1.y;
		float y2 = v3.y - v1.y;
		float z1 = v2.z - v1.z;
		float z2 = v3.z - v1.z;

		float s1 = w2.x - w1.x;
		float s2 = w3.x - w1.x;
		float t1 = w2.y - w1.y;
		float t2 = w3.y - w1.y;

		float r = 1.0F / (s1 * t2 - s2 * t1);
		Vector3D sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
			(t2 * z1 - t1 * z2) * r);
		Vector3D tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
			(s1 * z2 - s2 * z1) * r);

		tan1[i1] += sdir;
		tan1[i2] += sdir;
		tan1[i3] += sdir;

		tan2[i1] += tdir;
		tan2[i2] += tdir;
		tan2[i3] += tdir;

		triangle++;
	}

	long count = vertexCount;
	for (long a = 0; a < count; a++)
	{
		const Vector3D& n = normal[a];
		const Vector3D& t = tan1[a];

		// Gram-Schmidt orthogonalize
		tangent[a] = (t - n * Dot(n, t)).Normalize();

		// Calculate handedness
		tangent[a].w = (Dot(Cross(n, t), tan2[a]) < 0.0F) ? -1.0F : 1.0F;
	}

	delete[] tan1;
}
*/

void CModel::postLoad()
{
	/*
	meshList::iterator mit = m_meshes.begin();
	for(; mit != m_meshes.end(); ++mit)
	{
		CMesh& mesh = *mit;

		CMaterial_GLSL *pMat = dynamic_cast<CMaterial_GLSL*>(m_materials[mesh.m_materialIndex]);

		CVertexFormat vf(3,3,2);
		if(pMat)
		{
			CProgramObject& po = *pMat->getProgramObject();
			for(uint i = 0; i < header.numBones; ++i)
				m_boneSources[i]->setSkinMatrixUniform(CUniformHandle(po.getUniform<CMatrix4f>("skinMatrices["+toStr(i)+"]")));

			mesh.m_hwSkinning = true;
			vf.addShaderAttrib(po.getAttrib<CVector3f>("skinIndices"));
			vf.addShaderAttrib(po.getAttrib<CVector2f>("skinWeights"));
		}

		mesh.m_pVertexBuffer = IVertexBuffer::create(vf, numVertices);
		mesh.m_pVertexBuffer->lock();
		{
			vector<CSkinnedVertex>::iterator it = mesh.m_skinnedVertices.begin();
			CDataContainer<CVector3f>::iterator vit = mesh.m_pVertexBuffer->getVertices3f().begin();
			CDataContainer<CVector3f>::iterator nit = mesh.m_pVertexBuffer->getNormals3f().begin();
			CDataContainer<CVector2f>::iterator tit = mesh.m_pVertexBuffer->getTexCoords2f(0).begin();
			for(; it != mesh.m_skinnedVertices.end(); ++it, ++vit, ++nit, ++tit)
			{
				// put into vertexbuffer...
				*vit = it->p;
				*nit = it->n;
				*tit = it->;
			}
			BOOST_ASSERT(vit == mesh.m_pVertexBuffer->getVertices3f().end());
			if(pMat)
			{
				it = mesh.m_skinnedVertices.begin();
				CDataContainer<CVector3f>::iterator siit = mesh.m_pVertexBuffer->getAttrib3f(0).begin();
				CDataContainer<CVector2f>::iterator swit = mesh.m_pVertexBuffer->getAttrib2f(1).begin();
				for(; it != mesh.m_skinnedVertices.end(); ++it, ++siit, ++swit)
				{
					size_t numWeights = it->weights.size();
					if(numWeights == 1)
					{
						siit->set(float(it->weights[0].first)+0.5f, 0.0f, 0.0f);
						swit->set(it->weights[0].second, 0.0f);
					}
					else if(numWeights == 2)
					{
						siit->set(float(it->weights[0].first)+0.5f, float(it->weights[1].first)+0.5f, 0.0f);
						swit->set(it->weights[0].second, it->weights[1].second);
					}
					else if(numWeights == 3)
					{
						siit->set(float(it->weights[0].first)+0.5f, float(it->weights[1].first)+0.5f, float(it->weights[2].first)+0.5f);
						swit->set(it->weights[0].second, it->weights[1].second);
					}
					else
					{
						// FIXME: no good.
						siit->set(0.0f, 0.0f, 0.0f);
						swit->set(0.0f, 0.0f);
					}
				}
			}
		}
		mesh.m_pVertexBuffer->unlock();
	}
	*/
}

//////////////////////////////////////////////////////////////////////////

bool CModelImporter::canImport(std::string filename)
{
	FILE *inputFile = fopen(filename.c_str(), "rb");
	if(!inputFile)
		return false;

	MMFHeader header = freadtype<MMFHeader>(inputFile);
	fclose(inputFile);

	if(strncmp(header.identifier, "MMF", 3) == 0)
		return true;

	return false;
}

IModel* CModelImporter::load(std::string filename)
{
	return new CModel(filename);
}
