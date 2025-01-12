#pragma once

#include "IwEntity.h"

namespace iw {
namespace ECS {
	struct ComponentData {
		ref<void*[]> Components;

		template<
			typename _cs>
		_cs Tie() {
			return *reinterpret_cast<_cs*>(Components.get());
		}

		template<
			typename _c,
			size_t _i>
		_c* Get() {
			return reinterpret_cast<_c*>(Components[_i]);
		}
	};

	struct ComponentDataIndices {
		size_t Count;
		ref<size_t[]> Indices;
	};
}

	using namespace ECS;
}
