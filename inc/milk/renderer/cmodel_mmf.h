#ifndef MILK_CMODEL_MMF_H_
#define MILK_CMODEL_MMF_H_

#include "milk/renderer/imodel.h"

namespace milk
{
	namespace milkModelFile
	{
		// Animated component
		class CComponent
		{
			friend class CBoneSource;
			friend class CModel;
		public:
			float getValue(float frame) const;

		private:
			typedef std::map<float, CVector4f> keyframeList;
			keyframeList m_keyframes;
		};

		// Skeleton bone info
		class CBoneSource : public IBoneSource
		{
			friend class CModel;
		public:
			virtual ~CBoneSource()
			{ }

			CMatrix4f getBoneMatrix(float frame);
			CMatrix4f getBoneMatrixPose();
			CMatrix4f getWorldInv()
			{ return m_worldInv; }

		private:
			CMatrix4f m_worldInv;
			CVector3f m_translation, m_rotation, m_postRotation;
			CComponent m_components[6];
		};

		// Model
		class CModel : public IModel
		{
		public:
			CModel();
			CModel(std::string filename)
			{ load(filename); }

			virtual ~CModel()
			{ }

			bool load(std::string filename);

		private:
			void postLoad();
		};

		// Importer class
		class CModelImporter : public IModelImporter
		{
		public:
			bool canImport(std::string filename);
			IModel* load(std::string filename);
		};
	}
}

#endif
