#pragma once

#include "IwGraphics.h"
#include "iw/renderer/FrameBuffer.h"
#include "iw/renderer/Device.h"
#include "iw/util/memory/smart_pointers.h"
#include "Texture.h"

namespace IW {
namespace Graphics {
	struct IWGRAPHICS_API RenderTarget {
		int Width;
		int Height;

		Texture* Textures;
		size_t TextureCount;

		IFrameBuffer* Frame;

		RenderTarget(
			std::initializer_list<Texture> textures);

		RenderTarget(
			int width,
			int height,
			std::initializer_list<TextureFormat> formats,
			std::initializer_list<TextureFormatType> types);

		void Initialize(
			const iw::ref<IDevice>& device);

		void Use(
			const iw::ref<IDevice>& device);
	};
}

	using namespace Graphics;
}
