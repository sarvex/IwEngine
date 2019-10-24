#include "iw/renderer/Platform/OpenGL/GLDevice.h"
#include "iw/renderer/Platform/OpenGL/GLIndexBuffer.h"
#include "iw/renderer/Platform/OpenGL/GLVertexBuffer.h"
#include "iw/renderer/Platform/OpenGL/GLVertexArray.h"
#include "iw/renderer/Platform/OpenGL/GLVertexShader.h"
#include "iw/renderer/Platform/OpenGL/GLFragmentShader.h"
#include "iw/renderer/Platform/OpenGL/GLGeometryShader.h"
#include "iw/renderer/Platform/OpenGL/GLComputeShader.h"
#include "iw/renderer/Platform/OpenGL/GLPipeline.h"
#include "iw/renderer/Platform/OpenGL/GLComputePipeline.h"
#include "gl/glew.h"

namespace IwRenderer {
	IDevice* IDevice::Create() {
		return new GLDevice();
	}

	void GLDevice::DrawElements(
		MeshTopology topology,
		int count, 
		long long offset)
	{
		GLenum glTopology = 0;
		switch (topology) {
			case POINTS:    glTopology = GL_POINTS;	   break;
			case LINES:     glTopology = GL_LINES;     break;
			case TRIANGLES: glTopology = GL_TRIANGLES; break;
			case QUADS:     glTopology = GL_QUADS;	   break;
		}

		if (glTopology) {
			glDrawElements(
				glTopology,
				count,
				GL_UNSIGNED_INT,
				reinterpret_cast<const void*>(offset));
		}
	}

	IIndexBuffer* GLDevice::CreateIndexBuffer(
		size_t size,
		const void* data)
	{
		return new GLIndexBuffer(size, data);
	}

	void GLDevice::DestroyIndexBuffer(
		IIndexBuffer* indexBuffer)
	{
		delete indexBuffer;
	}

	void GLDevice::SetIndexBuffer(
		IIndexBuffer* indexBuffer)
	{
		static_cast<GLIndexBuffer*>(indexBuffer)->Bind();
	}

	IVertexBuffer* GLDevice::CreateVertexBuffer(
		size_t size,
		const void* data)
	{
		return new GLVertexBuffer(size, data);
	}

	void GLDevice::DestroyVertexBuffer(
		IVertexBuffer* vertexBuffer)
	{
		delete vertexBuffer;
	}

	void GLDevice::SetVertexBuffer(
		IVertexBuffer* vertexBuffer)
	{
		static_cast<GLVertexBuffer*>(vertexBuffer)->Bind();
	}

	void GLDevice::UpdateVertexBufferData(IVertexBuffer* buffer, size_t size, const void* data)
	{
	}

	IVertexArray* GLDevice::CreateVertexArray() {
		return new GLVertexArray();;
	}

	void GLDevice::DestroyVertexArray(
		IVertexArray* vertexArray) 
	{
		delete vertexArray;
	}

	void GLDevice::SetVertexArray(
		IVertexArray* vertexArray)
	{
		static_cast<GLVertexArray*>(vertexArray)->Bind();
	}

	void GLDevice::AddBufferToVertexArray(
		IVertexArray* vertexArray,
		IVertexBuffer* buffer,
		const VertexBufferLayout& layout)
	{
		static_cast<GLVertexArray*>(vertexArray)
			->AddBuffer(static_cast<GLVertexBuffer*>(buffer), layout);
	}

	void GLDevice::UpdateVertexArrayData(
		IVertexArray* vertexArray, 
		size_t bufferIndex, 
		size_t size, 
		const void* data)
	{
		static_cast<GLVertexArray*>(vertexArray)
			->UpdateBuffer(bufferIndex, size, data);
	}

	IVertexShader* GLDevice::CreateVertexShader(
		const char* source) 
	{
		return new GLVertexShader(source);
	}

	void GLDevice::DestroyVertexArray(
		IVertexShader* vertexShader)
	{
		delete vertexShader;
	}

	IFragmentShader* GLDevice::CreateFragmentShader(
		const char* source)
	{
		return new GLFragmentShader(source);
	}

	void GLDevice::DestroyFragmentShader(
		IFragmentShader* fragmentShader)
	{
		delete fragmentShader;
	}

	IGeometryShader* GLDevice::CreateGeometryShader(
		const char* source)
	{
		return new GLGeometryShader(source);
	}

	void GLDevice::DestroyGeometryShader(
		IGeometryShader* geometryShader)
	{
		delete geometryShader;
	}

	IComputeShader* GLDevice::CreateComputeShader(
		const char* source)
	{
		return new GLComputeShader(source);
	}

	void GLDevice::DestroyComputeShader(
		IComputeShader* computeShader)
	{
		delete computeShader;
	}

	IPipeline* GLDevice::CreatePipeline(
		IVertexShader* vertexShader, 
		IFragmentShader* fragmentShader,
		IGeometryShader* geometryShader)
	{
		return new GLPipeline(
			static_cast<GLVertexShader*>(vertexShader),
			static_cast<GLFragmentShader*>(fragmentShader),
			static_cast<GLGeometryShader*>(geometryShader));
	}

	void GLDevice::DestroyPipeline(
		IPipeline* pipeline)
	{
		delete pipeline;
	}

	void GLDevice::SetPipeline(
		IPipeline* pipeline)
	{
		static_cast<GLPipeline*>(pipeline)->Use();
	}

	IComputePipeline* GLDevice::CreateComputePipeline(
		IComputeShader* computeShader)
	{
		return new GLComputePipeline(
			static_cast<GLComputeShader*>(computeShader));
	}

	void GLDevice::DestroyComputePipeline(
		IComputePipeline* computePipeline)
	{
		delete computePipeline;
	}

	void GLDevice::SetComputePipeline(
		IComputePipeline* computePipeline)
	{
		static_cast<GLComputePipeline*>(computePipeline)->Use();
	}
}
