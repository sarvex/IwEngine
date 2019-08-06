#pragma once

#include "iw/engine/Layer.h"
#include "iw/entity/Space.h"
#include "iw/graphics/RenderQueue.h"
#include "iw/graphics/Loaders/ModelLoader.h"
#include "iw/renderer/Device.h"
#include "iw/renderer/Pipeline.h"

class GameLayer
	: public IwEngine::Layer
{
private:
	IwGraphics::ModelLoader loader;
	IwRenderer::IPipeline*  pipeline;

	//temp
	IwGraphics::ModelData* QuadData;
	IwGraphics::Mesh* QuadMesh;

public:
	GameLayer(
		IwEntity::Space& space,
		IwGraphics::RenderQueue& renderQueue);

	~GameLayer();

	int Initialize(
		IwEngine::InitOptions& options)  override;

	void Update()      override;
	void FixedUpdate() override;
	void ImGui()       override;
};
