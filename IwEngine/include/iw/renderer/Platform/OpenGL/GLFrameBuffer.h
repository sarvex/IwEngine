#pragma once

#include "GLTexture.h"
#include "iw/renderer/FrameBuffer.h"

namespace iw {
namespace RenderAPI {
	class GLFrameBuffer
		: public IFrameBuffer
	{
	private:
		unsigned gl_id;
		unsigned m_textureCount;
		bool m_noColor;

	public:
		IWRENDERER_API
		GLFrameBuffer(
			bool noColor);

		IWRENDERER_API
		~GLFrameBuffer();

		IWRENDERER_API
		void AttachTexture(
			GLTexture* texture);

		IWRENDERER_API
		void Bind() const;

		IWRENDERER_API
		void Unbind() const;

		IWRENDERER_API
		bool NoColor() const;
	};
}

	using namespace RenderAPI;
}
