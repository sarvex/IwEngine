#pragma once

#include "IwRenderer.h"
#include "iw/log/logger.h"
#include <vector>

namespace iw {
namespace RenderAPI {
	enum class BufferLayoutType {
		FLOAT,
		UINT,
		UCHAR
	};

	struct VertexBufferLayoutElement {
		BufferLayoutType BType;
		unsigned      Type;
		unsigned      Count;
		unsigned char Normalized;
	};

	class VertexBufferLayout;

	template<
		typename _t>
	VertexBufferLayout MakeLayout(
		unsigned count,
		unsigned instanceStride = 0)
	{
		VertexBufferLayout layout(instanceStride);
		layout.Push<_t>(count);

		return layout;
	}

	class VertexBufferLayout {
	private:
		std::vector<VertexBufferLayoutElement> m_elements;
		unsigned m_stride;
		unsigned m_instanceStride;

	public:
		IWRENDERER_API
		VertexBufferLayout(
			unsigned instanceStride = 0);

		template<
			typename _t>
		void Push(
			unsigned int count)
		{
			LOG_ERROR << "Cannot push invalid buffer type!";
			static_assert(false, "Cannot push invalid buffer type!");
		}

		template<>
		IWRENDERER_API
		void Push<float>(
			unsigned count);

		template<>
		IWRENDERER_API
		void Push<unsigned>(
			unsigned count);

		template<>
		IWRENDERER_API
		void Push<unsigned char>(
			unsigned count);

		IWRENDERER_API
		void Clear();

		IWRENDERER_API
		const std::vector<VertexBufferLayoutElement>& GetElements() const;

		IWRENDERER_API
		unsigned GetStride() const;

		IWRENDERER_API
		unsigned GetInstanceStride() const;
	};

	IWRENDERER_API
	unsigned int GetSizeOfType(
		unsigned int glType);
}

	using namespace RenderAPI;
}
