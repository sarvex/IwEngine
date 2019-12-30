#pragma once

#include "IwGraphics.h"
#include "iw/renderer/FrameBuffer.h"
#include "iw/renderer/Device.h"
#include "iw/util/memory/smart_pointers.h"
#include "Texture.h"

namespace IW {
namespace Graphics {
	struct RenderTarget {
	private:
		int m_width;
		int m_height;
		std::vector<iw::ref<Texture>> m_textures;

		IFrameBuffer* m_handle;

	public:
		IWGRAPHICS_API
		RenderTarget(
			int width,
			int height);

		IWGRAPHICS_API
		void AddTexture(
			iw::ref<Texture> texture);

		IWGRAPHICS_API
		void Initialize(
			const iw::ref<IDevice>& device);

		IWGRAPHICS_API
		void Use(
			const iw::ref<IDevice>& device);

		IWGRAPHICS_API
		int Width() const;

		IWGRAPHICS_API
		int Height() const;

		IWGRAPHICS_API
		const iw::ref<Texture>& Tex(
			int index) const;

		IWGRAPHICS_API
		IFrameBuffer* Handle() const;
	};
}

	using namespace Graphics;
}
