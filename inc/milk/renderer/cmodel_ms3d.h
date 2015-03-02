#ifndef MILK_CMODEL_MS3D_H_
#define MILK_CMODEL_MS3D_H_

#include "milk/renderer/imodel.h"

namespace milk
{
	class CModelImporterMS3D : public milk::IModelImporter
	{
	public:
		bool canImport(std::string filename);
		IModel* load(std::string filename);
	};
}

#endif
