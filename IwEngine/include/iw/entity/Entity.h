#pragma once

#include "iw/entity/Space.h"

namespace iw {
namespace ECS {
	struct REFLECT Entity {
		PASS_REFLECT EntityHandle Handle;
		iw::Space* Space;

		Entity()
			: Handle(EntityHandle::Empty)
			, Space(nullptr)
		{}

		Entity(
			EntityHandle handle,
			iw::Space* space
		)
			: Handle(handle)
			, Space(space)
		{}

		Entity(
			EntityHandle handle,
			ref<iw::Space>& space
		)
			: Handle(handle)
			, Space(space.get())
		{}

		bool operator==(
			const Entity& other) const
		{
			return Handle == other.Handle; // this was returning true when .Handle == .Handle was false?
		}

		bool operator!=(
			const Entity& other) const
		{
			return !operator==(other);
		}

		bool operator==(
			const EntityHandle& other) const
		{
			return Handle == other; // this was returning true when .Handle == .Handle was false?
		}

		bool operator!=(
			const EntityHandle& other) const
		{
			return !operator==(other);
		}

		operator bool() const {
			return *this != EntityHandle::Empty;
		}

		size_t Index() const {
			return Handle.Index;
		}

		short Version() const {
#ifdef IW_DEBUG
			if (!Space) {
				LOG_ERROR << "Entity has no space!";
				return -1;
			}
#endif

			return Space->GetEntity(Handle).Handle.Version;
		}

		bool Alive() const {
#ifdef IW_DEBUG
			if (!Space) {
				LOG_ERROR << "Entity has no space!";
				return false;
			}
#endif

			return Space->GetEntity(Handle).Handle.Alive;
		}

		// Invalidates entity index.
		void Destroy() {
#ifdef IW_DEBUG
			if (!Space) {
				LOG_ERROR << "Entity has no space!";
				return;
			}
#endif
			Space->ReviveEntity(Handle); // I think this needs to be alive to destroy, not sure how I feel about that
			Space->DestroyEntity(Handle);

			Handle = EntityHandle::Empty;
			Space = nullptr;
		}

		void Kill() {
#ifdef IW_DEBUG
			if (!Space) {
				LOG_ERROR << "Entity has no space!";
				return;
			}
#endif
			Space->KillEntity(Handle);
		}

		void Revive() {
#ifdef IW_DEBUG
			if (!Space) {
				LOG_ERROR << "Entity has no space!";
				return;
			}
#endif
			Space->ReviveEntity(Handle);
		}

		
		void Queue(
			const func_EntityChange& func) const
		{
#ifdef IW_DEBUG
			if (!Space) {
				LOG_ERROR << "Entity has no space!";
				return;
			}
#endif

			return Space->QueueEntity(Handle, func);
		}

		template<
			typename _c,
			typename... _args>
		_c* Add(
			_args&&... args)
		{
#ifdef IW_DEBUG
			if (!Space) {
				LOG_ERROR << "Entity has no space!";
				return nullptr;
			}
#endif
			if (Space->FindComponent<_c>(Handle)) {
				return Space->SetComponent<_c>(Handle, std::forward<_args>(args)...);
			}

			return Space->AddComponent<_c>(Handle, std::forward<_args>(args)...);
		}

		template<
			typename _c>
		void RemoveComponent() {
#ifdef IW_DEBUG
			if (!Space) {
				LOG_ERROR << "Entity has no space!";
				return;
			}
#endif
			Space->RemoveComponent<_c>(Handle);
		}

		template<
			typename _c,
			typename... _args>
		_c* Set(
			_args&&... args)
		{
#ifdef IW_DEBUG
			if (!Space) {
				LOG_ERROR << "Entity has no space!";
				return nullptr;
			}
#endif
			return Space->SetComponent<_c>(Handle, std::forward<_args>(args)...);
		}

		template<
			typename _c>
		_c* Find() const {
#ifdef IW_DEBUG
			if (!Space) {
				LOG_ERROR << "Entity has no space!";
				return nullptr;
			}
#endif
			return Space->FindComponent<_c>(Handle);
		}

		template<
			typename _c>
		bool Has() const {
#ifdef IW_DEBUG
			if (!Space) {
				LOG_ERROR << "Entity has no space!";
				return false;
			}
#endif
			return Space->HasComponent<_c>(Handle);
		}
	};

	const func_EntityChange func_Destroy = [](Entity e) {
		e.Destroy();
	};
}

	using namespace ECS;
}
