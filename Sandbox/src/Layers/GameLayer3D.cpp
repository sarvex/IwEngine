#include "Layers/GameLayer3D.h"
#include "iw/graphics/Loaders/ModelLoader.h"
#include "iw/engine/Components/CameraController.h"
#include "iw/engine/Components/Model.h"
#include "iw/input/Devices/Keyboard.h"
#include "iw/input/Devices/Mouse.h"
#include "iw/engine/Time.h"
#include "imgui/imgui.h"
#include "iw/graphics/MeshFactory.h"

#include "iw/util/io/File.h"

#include "Systems/BulletSystem.h"
#include "Systems/PlayerSystem.h"
#include "Systems/EnemySystem.h"

GameLayer3D::GameLayer3D(
	IwEntity::Space& space, 
	IW::Renderer& renderer,
	IW::AssetManager& asset)
	: Layer(space, renderer, asset, "Game")
{
	PushSystem<BulletSystem>();
	PushSystem<PlayerSystem>();
}

iwu::ref<IW::IPipeline> pipeline;

int GameLayer3D::Initialize(
	IwEngine::InitOptions& options)
{
	pipeline = Renderer.CreatePipeline("assets/shaders/pbr/pbrvs.glsl", "assets/shaders/pbr/pbrfs.glsl");

	IW::Mesh* mesh = IW::mesh_factory::create_uvsphere(24, 48);

	mesh->GenTangents();
	mesh->Initialize(Renderer.Device);

	material = std::make_shared<IW::Material>(pipeline);
	mesh->SetMaterial(material);

	iwu::ref<IW::Texture> albedo = Asset.Load<IW::Texture>("textures/metal/albedo.jpg");
	iwu::ref<IW::Texture> normal = Asset.Load<IW::Texture>("textures/metal/normal.jpg");
	//iwu::ref<IW::Texture> height = Asset.Load<IW::Texture>("textures/metal/height.png");
	iwu::ref<IW::Texture> metallic = Asset.Load<IW::Texture>("textures/metal/metallic.jpg");
	iwu::ref<IW::Texture> roughness = Asset.Load<IW::Texture>("textures/metal/roughness.jpg");
	iwu::ref<IW::Texture> ao = Asset.Load<IW::Texture>("textures/metal/ao.jpg");
	albedo->Initialize(Renderer.Device);
	normal->Initialize(Renderer.Device);
	//height->Initialize(Renderer.Device);
	metallic->Initialize(Renderer.Device);
	roughness->Initialize(Renderer.Device);
	ao->Initialize(Renderer.Device);

	PushSystem<EnemySystem>(mesh);

	material->SetTexture("albedoMap", albedo);
	material->SetTexture("normalMap", normal);
	//material->SetTexture("heightMap", height);
	material->SetTexture("metallicMap", metallic);
	material->SetTexture("roughnessMap", roughness);
	material->SetTexture("aoMap", ao);

	//material->SetFloats("albedo", &iwm::vector3(1.0f, 0.85f, 0.57f), 3);
	//material->SetFloat ("metallic", 1.0f);
	//material->SetFloat ("roughness", 0.6f);
	//material->SetFloat ("ao", 1.0f);
	material->Pipeline = pipeline;

	lightPositions[0] = iwm::vector3( 5,  5, 0);
	lightPositions[1] = iwm::vector3(-5,  5, 0);
	lightPositions[2] = iwm::vector3( 5, -5, 0);
	lightPositions[3] = iwm::vector3(-5, -5, 0);

	lightColors[0] = iwm::vector3(1);
	lightColors[1] = iwm::vector3(1, 0, 0);
	lightColors[2] = iwm::vector3(0, 1, 0);
	lightColors[3] = iwm::vector3(0, 0, 1);

	IW::Camera* perspective = new IW::PerspectiveCamera(0.17f, 1.778f, 0.001f, 1000.0f);
	//camera->Rotation = iwm::quaternion::create_from_euler_angles(0, iwm::IW_PI, 0);

	perspective->Position = iwm::vector3(0, 100, -5);
	perspective->Rotation = iwm::quaternion::create_from_euler_angles(-1.39f, 0, 0);

	IwEntity::Entity camera = Space.CreateEntity<IW::Transform, IwEngine::CameraController>();

	Space.SetComponentData<IwEngine::CameraController>(camera, perspective);

	IwEntity::Entity player = Space.CreateEntity<IW::Transform, IwEngine::Model, Player>();
	Space.SetComponentData<IW::Transform>(player, iwm::vector3(3, 0, 10));
	Space.SetComponentData<IwEngine::Model>(player, mesh, 1U);
	Space.SetComponentData<Player>(player, 10.0f, 100.0f, 0.1666f, 0.1f);

	IwEntity::Entity enemy = Space.CreateEntity<IW::Transform, IwEngine::Model, Enemy>();
	Space.SetComponentData<IW::Transform>  (enemy, iwm::vector3(0, 0, 10));
	Space.SetComponentData<IwEngine::Model>(enemy, mesh, 1U);
	Space.SetComponentData<Enemy>          (enemy, SPIN, 3.0f, 0.05f, 0.025f, 0.025f);

	return 0;
}

struct PlayerComponents {
	IW::Transform* Transform;
	IwEngine::CameraController* Controller;
};

struct TreeComponents {
	IW::Transform* Transform;
	IwEngine::Model* Model;
};

void GameLayer3D::PostUpdate() {
	for (auto entity : Space.Query<IwEngine::CameraController>()) {
		auto [controller] = entity.Components.TieTo<IwEngine::CameraController>();

		Renderer.BeginScene(controller->Camera);

		pipeline->GetParam("lightPositions")->SetAsFloats(&lightPositions, 4, 3); // need better way to pass scene data
		pipeline->GetParam("lightColors")   ->SetAsFloats(&lightColors, 4, 3);

		pipeline->GetParam("camPos") ->SetAsFloats(&controller->Camera->Position, 3);

		for (auto tree : Space.Query<IW::Transform, IwEngine::Model>()) {
			auto [transform, model] = tree.Components.Tie<TreeComponents>();

			for (size_t i = 0; i < model->MeshCount; i++) {
				Renderer.DrawMesh(transform, &model->Meshes[i]);
			}
		}

		Renderer.EndScene();
	}
}

void GameLayer3D::ImGui() {
	ImGui::Begin("Game layer");

	//ImGui::ColorPicker3("Color", (float*)std::get<0>(material->GetFloats("albedo")));
	//ImGui::SliderFloat("Metallic",  (float*)material->GetFloat("metallic"), 0, 1);
	//ImGui::SliderFloat("Roughness", (float*)material->GetFloat("roughness"), 0.3f, 1);
	//ImGui::SliderFloat("AO", (float*)material->GetFloat("ao"), 0, 1);
	//ImGui::ColorPicker4("Diffuse Color", (float*)&material->GetColor("diffuse"));
	//ImGui::ColorPicker4("Specular Color", (float*)&material->GetColor("specular"));

	ImGui::End();
}

bool GameLayer3D::On(
	IwEngine::MouseMovedEvent& event)
{
	//for (auto entity : Space.Query<IW::Transform, IwEngine::CameraController>()) {
	//	auto [transform, controller] = entity.Components.Tie<PlayerComponents>();

	//	float pitch = event.DeltaY * 0.0005f;
	//	float yaw   = event.DeltaX * 0.0005f;

	//	iwm::quaternion deltaP = iwm::quaternion::create_from_axis_angle(-transform->Right(), pitch);
	//	iwm::quaternion deltaY = iwm::quaternion::create_from_axis_angle( iwm::vector3::unit_y, yaw);

	//	transform->Rotation *= deltaP * deltaY;

	//	controller->Camera->Rotation = transform->Rotation;
	//}

	return false;
}

