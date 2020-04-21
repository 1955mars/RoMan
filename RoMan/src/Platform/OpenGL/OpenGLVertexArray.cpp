#include "rmpch.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

#include "RoMan/Renderer/Renderer.h"

namespace RoMan
{
	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
			case RoMan::ShaderDataType::Float:    return GL_FLOAT;
			case RoMan::ShaderDataType::Float2:   return GL_FLOAT;
			case RoMan::ShaderDataType::Float3:   return GL_FLOAT;
			case RoMan::ShaderDataType::Float4:   return GL_FLOAT;
			case RoMan::ShaderDataType::Mat3:     return GL_FLOAT;
			case RoMan::ShaderDataType::Mat4:     return GL_FLOAT;
			case RoMan::ShaderDataType::Int:      return GL_INT;
			case RoMan::ShaderDataType::Int2:     return GL_INT;
			case RoMan::ShaderDataType::Int3:     return GL_INT;
			case RoMan::ShaderDataType::Bool:     return GL_BOOL;
			case RoMan::ShaderDataType::Int4:     return GL_INT;
		}
		
		RM_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray()
	{
		RM_RENDER_S({ glCreateVertexArrays(1, &self->m_RendererID); });
	}
	OpenGLVertexArray::~OpenGLVertexArray()
	{
		RM_RENDER_S({ glDeleteVertexArrays(1, &self->m_RendererID); });
	}
	void OpenGLVertexArray::Bind() const
	{
		RM_RENDER_S({ glBindVertexArray(self->m_RendererID); });
	}
	void OpenGLVertexArray::UnBind() const
	{
		RM_RENDER({ glBindVertexArray(0); });
	}
	void OpenGLVertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer)
	{
		RM_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "VertexBuffer has no layout!");

		Bind();
		vertexBuffer->Bind();

		RM_RENDER_S1(vertexBuffer, {

		const auto & layout = vertexBuffer->GetLayout();
			for (const auto& element : layout)
			{
				auto glBaseType = ShaderDataTypeToOpenGLBaseType(element.Type);
				glEnableVertexAttribArray(self->m_VertexBufferIndex);
				if (glBaseType == GL_INT)
				{
					glVertexAttribIPointer(self->m_VertexBufferIndex,
						element.GetComponentCount(),
						glBaseType,
						layout.GetStride(),
						(const void*)(intptr_t)element.Offset);
				}
				else
				{
					glVertexAttribPointer(self->m_VertexBufferIndex,
						element.GetComponentCount(),
						glBaseType,
						element.Normalized ? GL_TRUE : GL_FALSE,
						layout.GetStride(),
						(const void*)(intptr_t)element.Offset);
				}
				self->m_VertexBufferIndex++;
			}
		});
		m_VertexBuffers.push_back(vertexBuffer);

	}
	void OpenGLVertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
	{
		Bind();
		indexBuffer->Bind();

		m_IndexBuffer = indexBuffer;
	}
}
