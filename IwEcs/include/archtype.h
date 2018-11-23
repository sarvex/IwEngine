#pragma once

#include <tuple>

namespace iwecs {
	template<typename... _components_t>
	class archtype {
	public:
		using tup_t = std::tuple<_components_t...>;
	private:
		tup_t m_components;
	public:
		archtype()
			: m_components() {}

		archtype(_components_t&&... args)
			: m_components(std::forward<_components_t&&>(args)...) {}

		//might need to be a tuple of references
		tup_t& components() {
			return m_components;
		}
	};
}