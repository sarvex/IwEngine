#pragma once

#include "IwGraphics.h"
#include "VertexBufferLayout.h"
#include "VertexBuffer.h"

namespace IwGraphics {
	class IWGRAPHICS_API VertexArray {
	private:
		unsigned int m_renderId;
		std::vector<VertexBuffer*> m_buffers;
		std::vector<VertexBufferLayout*> m_layouts;

	public:
		VertexArray();
		~VertexArray();

		void AddBuffer(
			VertexBuffer* vb,
			VertexBufferLayout* layout);

		void Bind() const;
		void Unbind() const;
	};
}
