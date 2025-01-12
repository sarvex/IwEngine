#include "iw/engine/Systems/Camera/EditorCameraControllerSystem.h"
#include "iw/input/Devices/Keyboard.h"
#include "iw/engine/Time.h"
#include <imgui/imgui.h>

namespace iw {
	EditorCameraControllerSystem::EditorCameraControllerSystem()
		: SystemBase("Editor Camera Controller")
		, speed(5.0f)
		, camera(nullptr)
		, cameraTransform(nullptr)
		, movement()
		, rotation()
	{}

	int EditorCameraControllerSystem::Initialize() {

		iw::Entity entity = Space->CreateEntity<Transform>();

		float aspect = float(Renderer->Width()) / Renderer->Height();

		if (MakeOrthoOnInit) camera = entity.Add<OrthographicCamera>(32 * aspect, 32, 0, 100);
		else                 camera = entity.Add<PerspectiveCamera> (1.7f, 1.777f, 0.01f, 1000.0f);

		cameraTransform = entity.Set<Transform>(glm::vec3(0, 10, 10), glm::vec3(1), glm::angleAxis(iw::Pi, glm::vec3(0, 1, 0)));

		camera->SetTrans(cameraTransform);

		return SystemBase::Initialize();
	}

	void EditorCameraControllerSystem::Update()
	{
		if (!Active) return;

		if (glm::length(movement) != 0) {
			glm::vec3 delta = glm::vec3();

			if (movement.x != 0) {
				delta += -cameraTransform->Right() * movement.x * speed * Time::DeltaTime();
			}

			if (movement.y != 0) {
				delta += glm::vec3(0, 1, 0) * movement.y * speed * Time::DeltaTime();
			}

			if (movement.z != 0) {
				glm::vec3 forward = cameraTransform->Forward();
				forward.y = 0;
				forward = glm::normalize(forward);

				if (glm::length2(forward) == 0) {
					forward = glm::cross(cameraTransform->Right(), glm::vec3(0, 1, 0));
				}

				delta += forward * movement.z * speed * Time::DeltaTime();
			}

			cameraTransform->Position += delta;
		}
			
		if (glm::length(rotation) != 0) {
			glm::quat deltaP = glm::angleAxis(-rotation.x, glm::vec3(0, 1, 0));
			glm::quat deltaY = glm::angleAxis( rotation.y, cameraTransform->Right());

			cameraTransform->Rotation = deltaP * deltaY * cameraTransform->Rotation;
		}

		rotation = glm::vec3(); // reset rotation
	}

	void EditorCameraControllerSystem::OnPush()
	{
		movement.y += Keyboard::KeyDown(InputName::SHIFT) ? -1 : 0;
		movement.y += Keyboard::KeyDown(InputName::SPACE) ?  1 : 0;
		
		movement.x += Keyboard::KeyDown(InputName::A) ? -1 : 0;
		movement.x += Keyboard::KeyDown(InputName::D) ?  1 : 0;
		
		movement.z += Keyboard::KeyDown(InputName::W) ?  1 : 0;
		movement.z += Keyboard::KeyDown(InputName::S) ? -1 : 0;
	}

	void EditorCameraControllerSystem::OnPop() {
		movement = glm::vec3(0);
	}

	bool EditorCameraControllerSystem::On(
		KeyEvent& e)
	{
		switch (e.Button)
		{
			case InputName::SHIFT: movement.y -= e.State ? 1 : -1; break;
			case InputName::SPACE: movement.y += e.State ? 1 : -1; break;

			case InputName::A:     movement.x -= e.State ? 1 : -1; break;
			case InputName::D:     movement.x += e.State ? 1 : -1; break;

			case InputName::W:     movement.z += e.State ? 1 : -1; break;
			case InputName::S:     movement.z -= e.State ? 1 : -1; break;
		}

		return false;
	}

	bool EditorCameraControllerSystem::On(
		MouseMovedEvent& e)
	{
		if (e.Device == DeviceType::RAW_MOUSE)
		{
			rotation.x = e.DeltaX * Time::DeltaTime();
			rotation.y = e.DeltaY * Time::DeltaTime();
		}

		return false;
	}

	bool EditorCameraControllerSystem::On(
		MouseButtonEvent& e)
	{
		if (e.Device == DeviceType::RAW_MOUSE) 
		{
			switch (e.Button)
			{
				case InputName::RMOUSE:
				{
					speed = e.State ? 50.0f : 5.0f;
					break;
				}
				case InputName::MMOUSE:
				{
					Active = e.State;
					LOG_INFO << "Editor camera controller is now " << (Active ? "non active" : "active");
					break;
				}
			}
		}

		return false;
	}

	bool EditorCameraControllerSystem::On(
		MouseWheelEvent& e)
	{
		if (e.Device == DeviceType::MOUSE)
		{
			cameraTransform->Position += e.Delta * cameraTransform->Forward() * speed;
		}

		return false;
	}
}
