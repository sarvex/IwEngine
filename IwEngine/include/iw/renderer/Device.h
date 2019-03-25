#pragma once

#include "IwRenderer.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "VertexShader.h"
#include "FragmentShader.h"
#include "GeometryShader.h"
#include "Pipeline.h"

namespace IwRenderer {
	class IWRENDERER_API IDevice {
	public:
		virtual void DrawElements(
			int count,
			long long offset) = 0;

		//Index buffers
		virtual IIndexBuffer* CreateIndexBuffer(
			size_t size,
			const void* data = nullptr) = 0;

		virtual void DestroyIndexBuffer(
			IIndexBuffer* indexBuffer) = 0;

		virtual void SetIndexBuffer(
			IIndexBuffer* indexBuffer) = 0;

		//Vertex buffers
		virtual IVertexBuffer* CreateVertexBuffer(
			size_t size,
			const void* data = nullptr) = 0;

		virtual void DestroyVertexBuffer(
			IVertexBuffer* vertexBuffer) = 0;

		virtual void SetVertexBuffer(
			IVertexBuffer* vertexBuffer) = 0;

		//Vertex arrays
		virtual IVertexArray* CreateVertexArray(
			size_t numBuffers,
			IVertexBuffer** vertexBuffers,
			VertexBufferLayout** vertexLayouts) = 0;

		virtual void DestroyVertexArray(
			IVertexArray* vertexArray) = 0;

		virtual void SetVertexArray(
			IVertexArray* vertexArray) = 0;

		//Vertex shader
		virtual IVertexShader* CreateVertexShader(
			const char* source) = 0;

		virtual void DestroyVertexArray(
			IVertexShader* vertexShader) = 0;

		//Fragment shader
		virtual IFragmentShader* CreateFragmentShader(
			const char* source) = 0;

		virtual void DestroyFragmentShader(
			IFragmentShader* fragmentShader) = 0;

		//Geometry shader
		virtual IGeometryShader* CreateGeometryShader(
			const char* source) = 0;

		virtual void DestroyGeometryShader(
			IGeometryShader* geometryShader) = 0;

		//Shader pipeline
		virtual IPipeline* CreatePipeline(
			IVertexShader* vertexShader,
			IFragmentShader* fragmentShader,
			IGeometryShader* geometryShader = nullptr) = 0;

		virtual void DestroyPipeline(
			IPipeline* pipeline) = 0;

		virtual void SetPipeline(
			IPipeline* pipeline) = 0;
	};
}