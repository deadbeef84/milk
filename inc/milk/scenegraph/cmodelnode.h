#ifndef MILK_CMODELNODE_H_
#define MILK_CMODELNODE_H_

#include <string>
#include <vector>
#include "milk/math/cvector.h"
#include "milk/math/cmatrix.h"
#include "milk/ccolor.h"
#include "milk/vectorpod.h"
#include "milk/boost.h"
#include "milk/scenegraph/irenderable.h"
#include "milk/renderer/imodel.h"

namespace milk
{
	class CSceneManager;

	/// A model node renders and animates a IModel.
	/**
	This uses a loaded IModel to render and animates it's vertices.
	Several CModelNode can share one IModel.
	*/
	class CModelNode : public ISceneNode
	{
	public:
		/// Create with no IModel
		CModelNode();

		/// Create with IModel
		CModelNode(IModel *pModel);

		/// Destructor
		virtual ~CModelNode();

		/// Set IModel "target". This is the model to render/animate. Null unloads the model.
		void setModel(IModel *pModel);

		/// Return pointer to IModel object, 0 if none has been set.
		IModel* getModel() const
		{ return m_pModel; }

		/// Check if a IModel has been set.
		bool hasModel() const;

		/// Render...
		virtual void render();

		/// Update animation...
		virtual void update(float dt);

		/// Render model
		/**
		Renders the model.
		*/
		void draw();

		/// Render stencil shadow
		//void drawShadow(const CVector3f& lightdir, float inf = 100.0f);

		/// Restart animation
		void restart();

		/// Set animation (for all bones) by name
		void setAnimation(std::string animation, float blendTime = -1.0f);

		/// Set animation (for all bones)
		void setAnimation(const CAnimation& animation, float blendTime = -1.0f);

		/// Set animation (by name) for a specific bone and it's children
		void setBoneAnimation(std::string bone, std::string animation, float blendTime = -1.0f);

		/// Set animation for a specific bone and it's children
		void setBoneAnimation(std::string bone, const CAnimation& animation, float blendTime = -1.0f);

		/// Advance animation
		/**
		Advance animation. This will update all animation joints.
		*/
		void advanceAnimation(float dt);

		/// Get animation bone/joint
		/**
		Find joint and return it's current matrix (ie. position and rotation)
		*/
		CBone* getBone(std::string name) const;

		const boneList& getBoneList() const
		{ return m_bones; }

		void setBindMaterial(bool bind)
		{ m_bindMaterial = bind; }

		bool getBindMaterial() const
		{ return m_bindMaterial; }

		void setAnimate(bool animate)
		{ m_animate = animate; }

		bool getAnimate() const
		{ return m_animate; }

		void setDrawSkeleton(bool drawSkeleton)
		{ m_drawSkeleton = drawSkeleton; }

		bool getDrawSkeleton() const
		{ return m_drawSkeleton; }

		void setDrawMesh(bool drawMesh)
		{ m_drawMesh = drawMesh; }

		bool getDrawMesh() const
		{ return m_drawMesh; }

	private:
		void drawSkeleton();

		// IModel pointer
		IModel *m_pModel;

		// Vector with joint info
		boneList m_bones;

		// Animation info
		float m_animationSpeed;
		float m_lastUpdate;

		// Drawing flags
		bool m_drawMesh;
		bool m_bindMaterial;
		bool m_animate;
		bool m_drawSkeleton;
	};
}

#endif
