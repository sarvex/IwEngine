#pragma once 

#include "iw/engine/Layer.h"
#include "iw/graphics/Font.h"
#include "iw/engine/Systems/Camera/EditorCameraControllerSystem.h"

#include "iw/graphics/RenderTarget.h"
#include "iw/graphics/Scene.h"

namespace iw {
	class ToolLayer
		: public Layer
	{
	private:
		Mesh plightMesh;
		Mesh dlightMesh;
		Mesh cameraMesh;

		ref<RenderTarget> selectionBuffer;

		Mesh textMesh;
		ref<Shader> fontShader;
		ref<Font> font;

		Camera* oldcamera;
		EditorCameraControllerSystem* cameraSystem;
		//Scene* m_mainScene;

	public:
		IWENGINE_API
		ToolLayer(/*Scene* scene*/);

		IWENGINE_API int  Initialize() override;
		IWENGINE_API void PostUpdate() override;

		IWENGINE_API void OnPush() override;
		IWENGINE_API void OnPop() override;
		IWENGINE_API void ImGui() override;

		IWENGINE_API
		bool On(MouseButtonEvent& e) override;
	};
}
