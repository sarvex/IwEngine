#include "Systems/ItemSystem.h"
#include "Events/ActionEvents.h"

#include "Components/Player.h"
#include "Components/Note.h"
#include "Components/Slowmo.h"

#include "iw/engine/Time.h"
#include "iw/physics/Collision/SphereCollider.h";
#include "iw/physics/Collision/CollisionObject.h";
#include "iw/audio/AudioSpaceStudio.h"
#include "iw/graphics/Model.h"

#include "iw/input/Devices/Keyboard.h"

ItemSystem::ItemSystem()
	: iw::System<iw::Transform, iw::CollisionObject, Item>("Item")
{
	m_prefab.Type = NOTE;
}

int ItemSystem::Initialize() {
	iw::MeshDescription   description = Asset->Load<iw::Model>("Sphere")->GetMesh(0).Data()->Description();
	iw::ref<iw::Material> material    = Asset->Load<iw::Model>("Sphere")->GetMesh(0).Material()->MakeInstance();

	iw::Mesh note = iw::MakePlane(description, 1, 1)->MakeInstance();

	material->Set("baseColor", iw::Color::From255(209, 195, 167));
	material->SetCastShadows(false);
	note.SetMaterial(material);

	m_noteMesh = note;

	return 0;
}

void ItemSystem::Update(
	iw::EntityComponentArray& view)
{
	for (auto entity : view) {
		auto [transform, object, item] = entity.Components.Tie<Components>();

		if (   item->Type == NOTE
			|| Space->HasComponent<Note>(entity.Handle))
		{
			Note* note = Space->FindComponent<Note>(entity.Handle);

			// open a note and pause the game, wait for x to resume

			if (note->Timer == 0.0f) {
				Bus->push<GameStateEvent>(PAUSED);
			}

			note->Timer += iw::Time::DeltaTime();

			if (note->Timer > note->Time) {
				if (iw::Keyboard::KeyDown(iw::X)) {
					QueueDestroyEntity(entity.Index);
					Bus->push<GameStateEvent>(RUNNING);
				}
			}
		}

	 if (   item->Type == CONSUMABLE
		 || Space->HasComponent<Slowmo>(entity.Handle))
	 {
			Slowmo* item = Space->FindComponent<Slowmo>(entity.Handle);

			if (item->Timer <= 0) {
				iw::SetTimeScale(1.0f);
			}

			if (item->Timer > 0) {
				item->Timer -= iw::Time::DeltaTime();
				iw::Time::SetTimeScale(0.1f);
			}

			else if (iw::Keyboard::KeyDown(iw::C)) {
				if (item->IsActive) {
					item->Timer = item->Time;
				}

				if (item->IsPickedup) {
					item->IsActive = true;
				}
			}
		}
	}
}

bool ItemSystem::On(
	iw::ActionEvent& e)
{
	switch (e.Action) {
		case iw::val(Actions::SPAWN_ITEM): {
			SpawnItemEvent& event = e.as<SpawnItemEvent>();
			
			iw::Transform* note = SpawnItem(m_prefab, event.Position);
			note->SetParent(event.Level);
			
			break;
		}
		case iw::val(Actions::SPAWN_NOTE): {
			SpawnNoteEvent& event = e.as<SpawnNoteEvent>();

			iw::Transform* note = SpawnNote(m_prefabs.at(event.Index));
			note->SetParent(m_root);
		
			break;
		}
	}

	return false;
}

bool ItemSystem::On(
	iw::CollisionEvent& e)
{
	iw::Entity a = Space->FindEntity(e.ObjA);
	if (a == iw::EntityHandle::Empty) {
		a = Space->FindEntity<iw::Rigidbody>(e.ObjA);
	}

	iw::Entity b = Space->FindEntity(e.ObjB);
	if (b == iw::EntityHandle::Empty) {
		b = Space->FindEntity<iw::Rigidbody>(e.ObjB);
	}

	iw::Entity note;
	iw::Entity other;
	if (   a != iw::EntityHandle::Empty
		&& a.Has<Item>())
	{
		note  = a;
		other = b;
	}

	else if (b != iw::EntityHandle::Empty
		&&   b.Has<Item>())
	{
		note  = b;
		other = a;
	}

	if (   other != iw::EntityHandle::Empty
		&& other.Has<Player>())
	{
		Bus->push<SpawnNoteEvent>(0);

		note.FindComponent<iw::Transform>()->SetParent(nullptr);
		Space->DestroyEntity(note.Index());
	}

	return false;
}

iw::Transform* ItemSystem::SpawnItem(
	Item prefab,
	iw::vector3 position)
{
	iw::Entity note = Space->CreateEntity<iw::Transform, iw::Mesh, iw::SphereCollider, iw::CollisionObject, Item>();

	iw::Transform*       t = note.Set<iw::Transform>(position, iw::vector3(0.65f, 0.9f, 0.9f), iw::quaternion::from_euler_angles(0, iw::Pi * 2 * rand() / RAND_MAX, 0));
	iw::Mesh*            m = note.Set<iw::Mesh>(m_noteMesh);
	iw::SphereCollider*  s = note.Set<iw::SphereCollider>(iw::vector3::zero, 1);
	iw::CollisionObject* c = note.Set<iw::CollisionObject>();
	                         note.Set<Item>(prefab);

	c->SetCol(s);
	c->SetTrans(t);
	c->SetIsTrigger(true);

	Physics->AddCollisionObject(c);

	return t;
}

iw::Transform* ItemSystem::SpawnNote(
	Note prefab)
{
	iw::Mesh mesh = m_font->GenerateMesh(prefab.Text, 0.005f, 1.0f);
	mesh.SetMaterial(m_material);

	iw::Entity note = Space->CreateEntity<iw::Transform, iw::Mesh, iw::UiElement, Note>();

	iw::Transform* t = note.Set<iw::Transform>(iw::vector3(-5, 3, 0));
	note.Set<iw::Mesh>(mesh);
	note.Set<Note>(prefab);

	return t;
}
