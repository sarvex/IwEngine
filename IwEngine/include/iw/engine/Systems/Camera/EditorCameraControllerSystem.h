#pragma once

#include "iw/engine/Core.h"
#include "iw/engine/System.h"
#include "iw/engine/Systems/Camera/CameraController.h"

namespace iw {
namespace Editor {
	class EditorCameraControllerSystem
		: public SystemBase
		, public CameraController
	{
	private:
		glm::vec3 movement;
		glm::vec2 rotation;
		float speed;

		Camera* camera; // Creates but doesnt own? idk if the system should delete this or not
		iw::Transform* cameraTransform;

	public:
		IWENGINE_API
		EditorCameraControllerSystem();

		int Initialize() override;

		void Update() override;
		void OnPush() override;
		void OnPop()  override;

		bool On(KeyEvent& e) override;
		bool On(MouseMovedEvent& e) override;
		bool On(MouseButtonEvent& e) override;
		bool On(MouseWheelEvent& e) override;

		Camera* GetCamera() override {
			return camera;
		}
	};
}

	using namespace Editor;
}
