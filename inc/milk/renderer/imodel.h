#ifndef MILK_CMODEL_H_
#define MILK_CMODEL_H_

#include <string>
#include <vector>
#include "milk/renderer/ivertexbuffer.h"
#include "milk/renderer/iindexbuffer.h"
#include "milk/renderer/ctexture.h"
#include "milk/renderer/cappearance.h"
#include "milk/math/cvector.h"
#include "milk/math/cmatrix.h"
#include "milk/timer.h"
#include "milk/ccolor.h"
#include "milk/vectorpod.h"
#include "milk/boost.h"
#include "milk/scenegraph/iscenenode.h"
#include "milk/iresource.h"

namespace milk
{
	/// Holds the weight-info. first is bone-index, and second is weight
	typedef std::pair<int, float> weight;
	/// List of weights
	typedef std::vector<weight> weightList;

	// Skinned vertex...
	class CSkinnedVertex
	{
	public:
		CVector3f p;
		CVector3f n;
		weightList weights;
	};

	// Mesh
	class CMesh
	{
	public:
		CMesh()
			: m_hwSkinning(false), m_skinned(false),
			m_materialIndex(0), m_renderMode(0),
			m_pVertexBuffer(0), m_pIndexBuffer(0)
		{ }

		~CMesh()
		{
			if(m_pVertexBuffer)
			{
				delete m_pVertexBuffer;
				m_pVertexBuffer = 0;
			}
			if(m_pIndexBuffer)
			{
				delete m_pIndexBuffer;
				m_pIndexBuffer = 0;
			}
		}

		bool m_hwSkinning;
		bool m_skinned;
		int m_materialIndex;
		GLenum m_renderMode;
		std::vector<CSkinnedVertex> m_skinnedVertices;
		milk::IVertexBuffer *m_pVertexBuffer;
		milk::IIndexBuffer *m_pIndexBuffer;
	};

	// Animation
	class CAnimation
	{
	public:
		CAnimation()
			: m_startFrame(0), m_endFrame(0), m_fps(24.0f), m_loop(true)
		{ }

		CAnimation(std::string name, float sf, float ef, float fps = 24.0f, bool loop = true)
			: m_name(name), m_startFrame(sf), m_endFrame(ef), m_fps(fps), m_loop(loop)
		{ }

		CAnimation(float sf, float ef, float fps = 24.0f, bool loop = true)
			: m_startFrame(sf), m_endFrame(ef), m_fps(fps), m_loop(loop)
		{ }

		std::string m_name;
		float m_startFrame;
		float m_endFrame;
		float m_fps;
		bool m_loop;
	};

	/// Feeds the bones with matrices
	class IBoneSource
	{
		friend class CBone;
		friend class IModel;
		friend class CModelNode;
	public:
		IBoneSource()
			: m_parent(-1), m_skinFrame(-1.0f)
		{ }

		virtual ~IBoneSource()
		{ }

		virtual CMatrix4f getBoneMatrix(float frame) = 0;
		virtual CMatrix4f getBoneMatrixPose() = 0;
		virtual CMatrix4f getWorldInv() = 0;

		void setSkinMatrix(const CMatrix4f& mat)
		{ m_skinMatrixUniform.set(mat); }

		void setSkinMatrixUniform(CUniformHandle smu)
		{ m_skinMatrixUniform = smu; }

	protected:
		std::string m_name;
		int m_parent;
		float m_skinFrame;
		CUniformHandle m_skinMatrixUniform;
	};

	/// Skeleton bone
	class CBone : public ISceneNode
	{
		friend class CModelNode;
		friend class IModel;
	public:
		CBone(IBoneSource *pBoneSource)
			: m_pBoneSource(pBoneSource), m_frame(-1.0f), m_blendTime(-1.0f), m_totalBlendTime(1.0f)
		{ }

		const std::string& getName() const
		{ return m_pBoneSource->m_name; }

		void setAnimation(const CAnimation& animation, float blendTime=-1.0f, bool children = true);

		bool atEnd() const;

		void updateBone(float dt);

		CBone* getParentBone() const
		{ return dynamic_cast<CBone*>(m_pParent); }

		IBoneSource* getBoneSource() const
		{ return m_pBoneSource; }

	protected:
		IBoneSource *m_pBoneSource;

		CAnimation m_animation;
		float m_frame;
		float m_blendTime;
		float m_totalBlendTime;

		CMatrix4f m_modelRelative;
		CMatrix4f m_transform;
		CMatrix4f m_blendFrom, m_blendTo;
	};

	typedef std::vector<CBone*> boneList;

	class IModelImporter;

	/// Modelclass for loading and rendering models
	/**
		This class currently supports MilkShape models, including animation.
		Note that this class only hold the model info, you'll need a CModelNode
		to render and animate the model.
	*/
	class IModel : public IResource
	{
		friend class CModelNode;
	public:

		static void init();
		static void free();
		static IModel* create(std::string filename);

		////////////////////////////////////

		/// Create empty model
		IModel()
		{ }

		virtual ~IModel()
		{ unload(); }

		MILK_MAKE_RESOURCE(IModel);

		////////////////////////////////////

		/// Load model from file
		virtual bool load(std::string filename)=0;

		/// Unload model
		virtual void unload();

		/// check if a model is loaded
		bool loaded() const;

		////////////////////////////////////

		/// Load animation from xml-file
		/**
			This will load animation-info from a file.
			It should be in this format:

			<mainsection>
				<section>
					<start>startframe</start>
					<end>endframe</end>
					<fps>frames-per-second</fps>
				</section>
			</mainsection>

			The fps tag is optional.
		*/
		void loadAnimations(std::string filename, std::string section, std::string mainsection = "animations");

		/// Get animation info from name
		const CAnimation& getAnimation(std::string name) const;

		/// Get animation info from name
		CAnimation& getAnimation(std::string name);

		/// Creates the skeleton (ie bone hierarchy)
		void createSkeleton(boneList& bones);

		////////////////////////////////////

		size_t bufferSize() const;
		size_t numMeshes() const;
		size_t numTriangles() const;
		size_t numDegenerateTriangles() const;
		size_t numVertices() const;
		size_t numIndices() const;
		size_t numMaterials() const;
		size_t numBones() const;

	protected:

		void skinVertices(CMesh& mesh, boneList& bones);
		void draw(CModelNode *pModelNode, boneList& bones);

		typedef std::vector<CMesh> meshList;
		typedef std::vector<handle<CAppearance> > materialList;
		typedef std::vector<CAnimation> animationList;
		typedef std::vector<IBoneSource*> boneSourceList;
		typedef std::vector<IModelImporter*> importerList;

		meshList m_meshes;
		materialList m_materials;
		animationList m_animations;
		boneSourceList m_boneSources;

		static importerList ms_importers;

	private:
		bool m_boundMaterial;
	};

	/// TODO
	class IModelImporter
	{
	public:
		virtual bool canImport(std::string filename) = 0;
		virtual IModel* load(std::string filename) = 0;
	};
}

#endif
