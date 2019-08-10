#include "iw/engine/EntryPoint.h"
#include "Layers/GameLayer.h"

#include "imgui/imgui.h"

#include "iw/entity2/Space.h"
#include "iw/util/memory/pool_allocator.h"

#include "iw/engine/Time.h"

struct int3 {
	int x, y, z;
};

struct float3 {
	float x, y, z;
};

struct long3 {
	long x, y, z;
};


class Game
	: public IwEngine::Application
{
private:
	IwEntity2::Space space;

public:
	Game() {
		InputManager.CreateDevice<IwInput::Mouse>();
		//InputManager.CreateDevice<IwInput::RawKeyboard>();

		PushLayer<GameLayer>();
	}

	int Initialize(
		IwEngine::InitOptions& options) override
	{
		Application::Initialize(options);

		ImGui::SetCurrentContext((ImGuiContext*)options.ImGuiContext);

		IwEngine::Time::Update();

		for (size_t i = 0; i < 1000000; i++) {
			IwEntity2::Entity e = space.CreateEntity();
			space.CreateComponent<int3>(e);
			space.CreateComponent<float3>(e);
			space.CreateComponent<long3>(e);

/*
			if (rand() < RAND_MAX / 2) {
				space.CreateComponent<float3>(e);
			}

			if (rand() < RAND_MAX / 4) {
				space.CreateComponent<long3>(e);
			}*/
		}

		IwEngine::Time::Update();
		float t = IwEngine::Time::DeltaTime();

		LOG_INFO << t;

		space.EntityExists(0);

		return 0;
	}
};

IwEngine::Application* CreateApplication(
	IwEngine::InitOptions& options)
{
	options.WindowOptions = IwEngine::WindowOptions {
		1280,
		720,
		true,
		IwEngine::NORMAL,
	};

	return new Game();
}
