#pragma once

#include "iw/engine/Events/Seq/EventSequence.h"
#include "iw/engine/Events/Events.h"

// default includes
#include "iw/engine/Time.h"
#include "iw/engine/ComponentHelper.h"

#include "iw/physics/Collision/MeshCollider.h" // temo for AddComponentToPhysicsEntity (bad)

#include <queue>
#include <string>

namespace iw {
namespace Engine {
	class ISystem {
	public:
		bool IsInitialized = false;

		virtual int  Initialize() = 0;
		virtual void Destroy() = 0;
		virtual void Update() = 0;
		virtual void FixedUpdate() = 0;
		virtual void ImGui() = 0;

		virtual void OnPush() = 0;
		virtual void OnPop() = 0;

		// Action events

		virtual bool On(ActionEvent& e) = 0;

		// Input events

		virtual bool On(MouseWheelEvent& e) = 0;
		virtual bool On(MouseMovedEvent& e) = 0;
		virtual bool On(MouseButtonEvent& e) = 0;
		virtual bool On(KeyEvent& e) = 0;
		virtual bool On(KeyTypedEvent& e) = 0;

		// Window events

		virtual bool On(WindowResizedEvent& e) = 0;

		// Os events

		virtual bool On(OsEvent& e) = 0;

		// Physics events

		virtual bool On(CollisionEvent& e) = 0;

		// Entity events

		virtual bool On(EntityCreatedEvent& e) = 0;
		virtual bool On(EntityDestroyEvent& e) = 0;
		virtual bool On(EntityDestroyedEvent& e) = 0;
		virtual bool On(EntityMovedEvent& e) = 0;

		virtual const std::string& Name() const = 0;
	};

	class SystemBase
		: public ISystem
	{
	private:
		std::string m_name;

	protected:
		APP_VARS

	public:
		SystemBase(
			std::string name)
			: m_name(name)
		{}

		virtual int  Initialize() { IsInitialized = true; return 0; }
		virtual void Destroy() { IsInitialized = false; }
		virtual void Update() {}
		virtual void FixedUpdate() {}
		virtual void ImGui() {}

		virtual void OnPush() override {}
		virtual void OnPop()  override {}

		// Action Events

		virtual bool On(ActionEvent& e) override { return false; }

		// Input events

		virtual bool On(MouseWheelEvent& e)  override { return false; }
		virtual bool On(MouseMovedEvent& e)  override { return false; }
		virtual bool On(MouseButtonEvent& e) override { return false; }
		virtual bool On(KeyEvent& e)         override { return false; }
		virtual bool On(KeyTypedEvent& e)    override { return false; }

		// Window events

		virtual bool On(WindowResizedEvent& e) override { return false; }

		// Os events

		virtual bool On(OsEvent& e) override { return false; }

		// Physics events

		virtual bool On(CollisionEvent& e) override { return false; }

		// Entity events

		virtual bool On(EntityDestroyEvent& e)   override { return false; }
		virtual bool On(EntityDestroyedEvent& e) override { return false; }
		virtual bool On(EntityMovedEvent& e)     override { return false; }
		virtual bool On(EntityCreatedEvent& e)   override { return false; }

		const std::string& Name() const override {
			return m_name;
		}
	protected:

		// should find a way to batch this
		template<
			typename _c,
			typename... _args>
		_c* AddComponentToPhysicsEntity(
			Entity& entity,
			_args&&... args)
		{
			CollisionObject* obj = Space->FindComponent<CollisionObject>(entity.Handle);
			if (!obj) obj = Space->FindComponent<Rigidbody>(entity.Handle);

			Physics->RemoveCollisionObject(obj);

			_c* component = Space->AddComponent<_c>(entity.Handle, args...);

			obj = Space->FindComponent<CollisionObject>(entity.Handle);
			if (!obj) obj = Space->FindComponent<Rigidbody>(entity.Handle);

			obj->Collider = Space->FindComponent<MeshCollider2>(entity.Handle); // pass by template?

			if (obj->IsDynamic) Physics->AddRigidbody((Rigidbody*)obj);
			else				Physics->AddCollisionObject(obj);

			return component;
		}

		template<
			typename _t1,
			typename _t2 = void>
		bool GetEntitiesFromManifold(
			const Manifold& manifold,
			Entity& e1,
			Entity& e2)
		{
			return iw::GetEntitiesFromManifold<_t1, _t2>(Space, manifold, e1, e2);
		}

		inline CollisionObject* GetPhysicsComponent(
			EntityHandle e)
		{
			return iw::GetPhysicsComponent(Space, e);
		}

		inline EventSequence CreateSequence() {
			EventSequence seq;
			seq.SetAppVars(MakeAppVars());

			return seq;
		}
	private:
		friend class Layer;

		SET_APP_VARS
		MAKE_APP_VARS
	};

	template<
		typename... _cs>
	class System
		: public SystemBase
	{
	protected:
		virtual void Update(
			EntityComponentArray& view)
		{}

		virtual void FixedUpdate(
			EntityComponentArray& view)
		{}
	public:
		System(
			const char* name)
			: SystemBase(name)
		{}

		virtual int Initialize() {
			return 0;
		}

		// These wont have to be copies this is just temp

		void Update() override {
			EntityComponentArray eca = Space->Query<_cs...>();
			// Break up view into Viewlets to execute on seperate threads

			// Execute threads
			Update(eca);
		}

		void FixedUpdate() override {
			EntityComponentArray eca = Space->Query<_cs...>();
			// Break up view into Viewlets to execute on seperate threads

			// Execute threads
			FixedUpdate(eca);
		}
	};
}

	using namespace Engine;
}
