#pragma once

#include "iw/engine/System.h"
#include <vector>

#include "iw/common/Components/Transform.h"
#include "iw/graphics/Model.h"
#include "iw/graphics/ParticleSystem.h"
#include "iw/physics/Collision/CollisionObject.h"
#include "iw/physics/Dynamics/Rigidbody.h"

#include "iw/physics/Collision/PlaneCollider.h"
#include "iw/physics/Collision/SphereCollider.h"
#include "iw/physics/Collision/CapsuleCollider.h"

namespace iw {
namespace Editor {
	class SpaceInspectorSystem
		: public SystemBase
	{
	private:
		std::vector<EntityHandle> m_entities;
		float m_timer;

		EntityHandle m_selectedEntity;
		ref<MeshData> m_selectedMeshData;

	public:
		IWENGINE_API SpaceInspectorSystem();

		IWENGINE_API void Update() override;
		IWENGINE_API void ImGui()  override;
	private:
		void AddEntityToHierarchy(
			EntityHandle& handle,
			iw::Transform* cache);

		void PrintTransform(iw::Transform* transform, bool showParent = true);
		void PrintMesh(iw::Mesh* mesh);
		void PrintModel(iw::Model* model);
		void PrintMaterial(iw::Material* material);
		void PrintParticleSystem(iw::ParticleSystem<>* system);
		void PrintCollisionObject(iw::CollisionObject* object);
		void PrintRigidbody(iw::Rigidbody* body);

		template<
			Dimension _d>
		void PrintPlaneCollider(
			iw::Physics::impl::PlaneCollider<_d>* collider)
		{
			if (!collider) return;

			if (ImGui::BeginTable("Plane Collider", 2)) {
				ImGui::TableSetupColumn("", 0, 0.3f);
				ImGui::TableSetupColumn("", 0, 0.7f);

				PrintCell("Normal");   PrintEditCell(&collider->Normal);
				PrintCell("Distance"); PrintEditCell(&collider->Distance);

				ImGui::EndTable();
			}
		}

		template<
			Dimension _d>
		void PrintSphereCollider(
			iw::Physics::impl::SphereCollider<_d>* collider)
		{
			if (!collider) return;

			if (ImGui::BeginTable("Sphere Collider", 2)) {
				ImGui::TableSetupColumn("", 0, 0.3f);
				ImGui::TableSetupColumn("", 0, 0.7f);

				PrintCell("Center"); PrintEditCell(&collider->Center);
				PrintCell("Radius"); PrintEditCell(&collider->Radius);

				ImGui::EndTable();
			}
		}

		template<
			Dimension _d>
		void PrintCapsuleCollider(
			iw::Physics::impl::CapsuleCollider<_d>* collider)
		{
			if (!collider) return;

			if (ImGui::BeginTable("Capsule Collider", 2)) {
				ImGui::TableSetupColumn("", 0, 0.3f);
				ImGui::TableSetupColumn("", 0, 0.7f);

				PrintCell("Center"); PrintEditCell(&collider->Center);
				PrintCell("Height"); PrintEditCell(&collider->Height);
				PrintCell("Radius"); PrintEditCell(&collider->Radius);

				ImGui::EndTable();
			}
		}

		template<
			typename _t>
		void PrintCell(
			_t t)
		{
			std::stringstream ss;
			ss << t;

			ImGui::TableNextCell();
			ImGui::Text(ss.str().c_str());
		}

		template<
			typename _t>
		void PrintHeader(
			_t t)
		{
			std::stringstream ss;
			ss << t;

			//ImGui::TableNextCell();
			ImGui::TableSetupColumn(ss.str().c_str());
		}

		template<
			typename _t>
		void PrintEditCell(
			_t* value,
			float min = 0, // should get form _t
			float max = 0,
			float rate = 0.1f)
		{
			ImGui::TableNextCell();
			ImGui::PushID((void*)value);

			     if constexpr (std::is_same_v<float, _t>)     ImGui::DragFloat ("", (float*)value, rate, min, max);
			else if constexpr (std::is_same_v<bool,  _t>)     ImGui::Checkbox  ("", (bool*) value);
			else if constexpr (std::is_same_v<glm::vec2, _t>) ImGui::DragFloat2("", (float*)value, rate, min, max);
			else if constexpr (std::is_same_v<glm::vec3, _t>) ImGui::DragFloat3("", (float*)value, rate, min, max);
			else if constexpr (std::is_same_v<glm::vec4, _t>
				           || std::is_same_v<glm::quat, _t>) ImGui::DragFloat4("", (float*)value, rate, min, max);

			ImGui::PopID();
		}
	};
}

	using namespace Editor;
}
