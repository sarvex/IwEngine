#pragma once

#include "iw/entity2/IwEntity.h"
#include "iw/util/set/sparse_set.h"
#include <type_traits>

namespace IwEntity2 {
	class IWENTITY2_API ArchetypeData {
	private:
		iwu::sparse_set<Component, size_t> m_components;
		//std::size_t            m_szie;

		friend class std::hash<ArchetypeData>;

	public:
		bool operator==(
			const ArchetypeData& other) const;

		void AddComponent(
			Component id,
			size_t size);

		void RemoveComponent(
			Component id);

		bool HasComponent(
			Component id);

		size_t SizeBefore(
			Component id) const;

		size_t SizeAfter(
			Component id) const;

		size_t Size()  const;
		size_t Count() const;
		bool   Empty() const;
		void   Reset();
	};
}
//
//namespace std {
//	template<>
//	struct hash<
//		IwEntity2::ArchetypeData>
//	{
//		size_t operator()(
//			const IwEntity2::ArchetypeData& archetype) const noexcept
//		{
//			auto& components = archetype.m_components;
//			size_t seed      = archetype.Count();
//			for (auto c : components) {
//				seed ^= c + 0x9e3779b9 + (seed << 6) + (seed >> 2);
//			}
//
//			return seed;
//		}
//	};
//}
