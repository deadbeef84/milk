#ifndef  MILK_ISCENENODE_H_
#define  MILK_ISCENENODE_H_

#include "milk/types.h"
#include "milk/error.h"
#include "milk/math/cmatrix4.h"
#include "milk/scenegraph/ctransform.h"
#include <list>
#include <set>

namespace milk
{
	const uint
		SCOPE_NONE = 0,
		SCOPE_3D = 1,
		SCOPE_2D = 2,
		SCOPE_SHADOW_CAST = 4,
		SCOPE_SHADOW_RECIEVE = 8,
		SCOPE_DEFAULT = 0xffff,
		SCOPE_ALL = 0xffffffff;

	class CSceneManager;

	/// This is the base-class for all scenegraph-nodes.
	class ISceneNode : public CTransform
	{
		friend class CSceneManager;
	public:
		typedef std::list<ISceneNode*> childList;

		enum LinkMode { DISABLED, INCLUDE, EXCLUDE };

		ISceneNode()
			: m_pParent(0), m_pSceneManager(0), m_scope(SCOPE_DEFAULT), m_visible(true), m_dirty(true), m_linkMode(DISABLED)
		{ ++ms_nodeCount; }

		virtual ~ISceneNode()
		{ --ms_nodeCount; }

		/// Total number of allocated scene-nodes.
		static size_t numNodes()
		{ return ms_nodeCount; }

		/// Adds a child to this node
		void addChild(ISceneNode *pChild)
		{
			addInternalChild(pChild);
			addRef();
		}

		/// Remove child from this node
		void removeChild(ISceneNode *pChild)
		{
			removeInternalChild(pChild);
			release();
		}

		/// Will remove this child from it's parent
		void makeOrphan()
		{
			if(m_pParent)
				m_pParent->removeChild(this);
		}

		/// Get the parent node of this node
		ISceneNode* getParentNode()
		{ return m_pParent; }

		/// Return the scenemanager which this node belongs to
		CSceneManager* getSceneManager()
		{ return m_pSceneManager; }

		/// Get parent node of a specific type
		template<class C>
		C* getParentByType()
		{
			ISceneNode *pNode = getParentNode();
			while(pNode != 0 && dynamic_cast<C*>(pNode) == 0)
				pNode = pNode->getParentNode();
			return static_cast<C*>(pNode);
		}

		/// Get the node closest to the root of a specific type
		template<class C>
		C* getRootByType()
		{
			ISceneNode *pRoot = dynamic_cast<C*>(this);
			ISceneNode *pNode = getParentNode();
			while(pNode != 0)
			{
				if(dynamic_cast<C*>(pNode) != 0)
					pRoot = pNode;
				pNode = pNode->getParentNode();
			}
			return static_cast<C*>(pRoot);
		}

		/// Get the root-node
		ISceneNode* getRootNode()
		{
			ISceneNode *pNode = this;
			while(pNode->m_pParent != 0)
				pNode = pNode->m_pParent;
			return pNode;
		}

		/// Render
		virtual void render()
		{ }

		/// Update all nodes
		virtual void animate(float dt)
		{
			for(childList::iterator it = m_children.begin(); it != m_children.end(); ++it)
				(*it)->animate(dt);
		}

		const childList& getChildren() const
		{ return m_children; }

		/// Set the scope (group) of this node.
		void setScope(uint scope)
		{ m_scope = scope; }

		/// Get the scope (group) of this node.
		uint getScope()
		{ return m_scope; }

		bool getVisible() const
		{ return m_visible; }

		void setVisible(bool visible)
		{ m_visible = visible; }









		// Get the local transformation matrix.
		CMatrix4f& ltm()
		{ updateLTM(); return m_ltm; }

		/*
		/// Multiply with the world transform
		void doTransform();

		/// Set triangle winding depending on transform and the active camera
		void setWinding();

		/// Get triangle winding depending on transform and the active camera
		bool getWinding();
		*/

		/// Marks this node and all of its children dirty! (for frames only)
		virtual void markDirty()
		{
			if(!m_dirty)
			{
				m_dirty = true;
				for(childList::iterator it = m_children.begin(); it != m_children.end(); ++it)
					(*it)->markDirty();
			}
		}

		/// Adjusts the MATRIX to conform to the (hopefully changed) current LTM.
		/**
		The new MATRIX is calculated like this:
		MATRIX = inverse(parent->LTM) * LTM

		Note: Actually it uses inverseFast() because I assume the matrices are OrthoNormalized.
		*/
		void uploadLTM();
















		/// Set the link-mode for the link-list.
		void setLinkMode(LinkMode mode)
		{ m_linkMode = mode; }
		LinkMode getLinkMode() const
		{ return m_linkMode; }

		void clearLinkNodes()
		{ m_linkNodes.clear(); }
		void addLinkNode(ISceneNode *pNode)
		{ m_linkNodes.insert(pNode); }
		void removeLinkNode(ISceneNode *pNode)
		{ m_linkNodes.erase(pNode); }
		bool hasLinkNode(ISceneNode *pNode) const
		{ return m_linkNodes.count(pNode) ? true : false; }
		bool nodeIsLinked(ISceneNode *pNode) const
		{
			if(m_linkMode == DISABLED)
				return true;
			else if(m_linkMode == INCLUDE)
				return hasLinkNode(pNode);
			else // EXCLUDE
				return !hasLinkNode(pNode);
		}

		static bool nodesAreLinked(ISceneNode *pLink1, ISceneNode *pLink2)
		{
			return pLink1->nodeIsLinked(pLink2) && pLink2->nodeIsLinked(pLink1);
		}

	protected:

		void renderRecursive()
		{
			if(m_visible)
			{
				render();
				for(childList::iterator it = m_children.begin(); it != m_children.end(); ++it)
					(*it)->renderRecursive();
			}
		}

		/// Updates the LTM (Local Transformation Matrix)
		/**
		The LTM is the matrix which transform this nodes data into world space.
		It's calculated like this:
		LTM = parent->LTM * MATRIX;
		*/
		void updateLTM();

		virtual void onDelete()
		{
			makeOrphan();
			setSceneManager(0);

			CTransform::onDelete();
		}

		/// Adds a child to this node (internal means no ref-counting)
		void addInternalChild(ISceneNode *pChild);

		/// Remove child from this node (internal means no ref-counting)
		void removeInternalChild(ISceneNode *pChild);

		/// Set scene-manager recursively for every children
		void setSceneManager(CSceneManager *pSceneManager);

		/// Add comments here TODO
		virtual void onNewSceneManager(CSceneManager *) {}
		virtual void onBecomeChild() {}
		virtual void onBecomeOrphan() {}
		virtual void onAddChild(ISceneNode*) {}
		virtual void onRemoveChild(ISceneNode*) {}

		ISceneNode *m_pParent;
		CSceneManager *m_pSceneManager;
		childList m_children;
		uint m_scope;
		bool m_visible;

		CMatrix4f m_ltm;
		bool m_dirty; // does the ltm need to be updated?

		LinkMode m_linkMode;
		std::set<ISceneNode*> m_linkNodes;

	private:
		static size_t ms_nodeCount;
	};
}

#endif
