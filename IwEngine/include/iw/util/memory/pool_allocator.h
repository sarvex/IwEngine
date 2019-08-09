#pragma once

#include "iw/util/iwutil.h"
#include <cstddef>
#include <malloc.h>
#include <forward_list>

namespace iwutil {
	class IWUTIL_API pool_allocator {
	private:
		class page {
		private:
			char* m_memory;
			page* m_next;
			std::forward_list<char*> m_freelist;

		public:
			page(
				size_t size,
				size_t count,
				size_t run);

			~page();

			void* alloc(
				size_t size,
				size_t run);

			bool free(
				void* addr,
				size_t size,
				size_t run);

			page(const page&)            = delete;
			page& operator=(const page&) = delete;
		};

		page* m_root;
		size_t m_pageSize;
		size_t m_itemSize;

	public:
		pool_allocator(
			size_t pageSize,
			size_t itemSize);

		pool_allocator(
			pool_allocator&&) noexcept;

		pool_allocator(
			const pool_allocator&) = delete;

		~pool_allocator();

		pool_allocator& operator=(
			pool_allocator&&) noexcept;

		pool_allocator& operator=(
			const pool_allocator&) = delete;

		template<
			typename _t>
		_t* alloc() {
			return (_t*)alloc();
		}

		void* alloc();

		bool free(
			void* addr);

		inline size_t page_size() const {
			return m_pageSize;
		}

		inline size_t item_size() const {
			return m_itemSize;
		}

		inline size_t page_capacity() const {
			return m_pageSize / m_itemSize;
		}
	};
}
