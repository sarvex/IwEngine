#include "iw/engine/Entity/EntityLayer.h"
#include "iw/graphics/Loaders/ModelLoader.h"
#include "iw/renderer/Platform/OpenGL/GLDevice.h"
#include "iw/renderer/Platform/OpenGL/GLPipeline.h"
#include "iw/log/logger.h"
#include "imgui/imgui.h"
#include "iw/util/io/File.h"

namespace IwEngine {
	EntityLayer::EntityLayer()
		: Layer("Entity")
		, viewTransform(iwm::matrix4::identity)
		, projTransform(iwm::matrix4::create_perspective_field_of_view(
			1.2f, 1.78f, 0.1f, 1000.0f))
		, lightColor(iwm::vector3::one)
		, lightAngle(0.0f)
		, specularScale(0.0f)
		, pos(0, -2, -10)
		, vel(0)
		, rot(0)
	{}

	EntityLayer::~EntityLayer() {}

	int EntityLayer::Initilize() {
		device = new IwRenderer::GLDevice();

		IwGraphics::ModelLoader loader;
		IwGraphics::ModelData* obj = loader.Load("res/tree.obj");

		for (size_t i = 0; i < obj->MeshCount; i++) {
			IwRenderer::IIndexBuffer* ib = device->CreateIndexBuffer(
				obj->Indices[i].FaceCount,
				obj->Indices[i].Faces);

			IwRenderer::VertexBufferLayout* vbl
				= new IwRenderer::VertexBufferLayout();
			vbl->Push<float>(3);
			vbl->Push<float>(3);

			IwRenderer::IVertexBuffer* vb = device->CreateVertexBuffer(
				obj->Meshes[i].VertexCount * sizeof(IwGraphics::Vertex),
				obj->Meshes[i].Vertices);

			IwRenderer::IVertexArray* va
				= device->CreateVertexArray(1, &vb, &vbl);

			model.push_back({ va, ib, obj->Indices[i].FaceCount });
		}

		IwRenderer::IVertexShader* vs = device->CreateVertexShader(
			IwUtil::ReadFile("res/defaultvs.glsl").c_str());

		IwRenderer::IFragmentShader* fs = device->CreateFragmentShader(
			IwUtil::ReadFile("res/defaultfs.glsl").c_str());

		pipeline = device->CreatePipeline(vs, fs);

		return 0;
	}

	void EntityLayer::Destroy() {
		for (auto& mesh : model) {
			device->DestroyVertexArray(mesh.Vertices);
			device->DestroyIndexBuffer(mesh.Indices);
		}

		device->DestroyPipeline(pipeline);
	}

	void EntityLayer::Update() {
		lightAngle += 0.005;

		float x = cos(lightAngle) * 100;
		float z = sin(lightAngle) * 100;

		modelTransform = iwm::matrix4::create_rotation_y(rot)
			* iwm::matrix4::create_translation(pos.x, pos.y, pos.z);

		device->SetPipeline(pipeline);

		pipeline->GetParam("model")
			->SetAsMat4(modelTransform);

		pipeline->GetParam("view")
			->SetAsMat4(viewTransform);

		pipeline->GetParam("proj")
			->SetAsMat4(projTransform);

		pipeline->GetParam("lightPos")
			->SetAsVec3(iwm::vector3(x, 0, z));

		pipeline->GetParam("lightColor")
			->SetAsVec3(lightColor);

		pipeline->GetParam("specularScale")
			->SetAsFloat(specularScale);

		for (auto& mesh : model) {
			device->SetVertexArray(mesh.Vertices);
			device->SetIndexBuffer(mesh.Indices);
			device->DrawElements(mesh.Count, 0);
		}

		pos += vel;
	}

	void EntityLayer::ImGui() {
		ImGui::Begin("Entity layer");

		ImGui::Text("Pos (x, y, z): %f %f %f", pos.x, pos.y, pos.z);
		ImGui::Text("Rot (y): %f", rot);

		ImGui::SliderFloat3("Light color", &lightColor.x, 0, 1);
		ImGui::SliderFloat("Specular scale", &specularScale, 0, 10);

		ImGui::End();
	}

	bool EntityLayer::On(
		WindowResizedEvent& event)
	{
		return false;
	}

	bool EntityLayer::On(
		MouseMovedEvent& event)
	{
		return false;
	}

	bool EntityLayer::On(
		MouseButtonEvent& event)
	{
		float speed = event.State ? .2f : 0.0f;
		if (event.Button == IwInput::MOUSE_L_BUTTON) {
			vel.z = -speed;
		}

		else if (event.Button == IwInput::MOUSE_R_BUTTON) {
			vel.z = speed;
		}

		else if (event.Button == IwInput::MOUSE_X1_BUTTON) {
			vel.x = speed;
		}

		else if (event.Button == IwInput::MOUSE_X2_BUTTON) {
			vel.x = -speed;
		}

		return false;
	}

	bool EntityLayer::On(
		MouseWheelEvent& event)
	{
		rot += event.Delta * .1f;

		return false;
	}
}