#pragma once

#include "iw/engine/Core.h"
#include "iw/graphics/Mesh.h"
#include "iw/graphics/Asset/ModelData.h"

namespace IwEngine {
	struct Model {
		IwGraphics::ModelData* Data;
		IwGraphics::Mesh*      Meshes;
		std::size_t            MeshCount;
	};
}
