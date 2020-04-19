#include "rmpch.h"
#include "Renderer.h"

#include "Shader.h"

namespace RoMan
{
	Renderer::SceneData* Renderer::s_SceneData = new Renderer::SceneData;
	Renderer* Renderer::s_Instance = new Renderer();

	void Renderer::Init()
	{
		s_Instance->m_ShaderLibrary = std::make_unique<ShaderLibrary>();
		RM_RENDER({ RendererAPI::Init(); });

		//RenderCommand::Init();
		Renderer::GetShaderLibrary()->Load("assets/shaders/HazelPBRStatic.glsl");
	}

	void Renderer::BeginScene(Camera& camera)
	{
		s_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::Clear()
	{
		RM_RENDER({
			RendererAPI::Clear(0.0f, 0.0f, 0.0f, 1.0f);
			});
	}

	void Renderer::Clear(float r, float g, float b, float a)
	{
		RM_RENDER_4(r, g, b, a, {
			RendererAPI::Clear(r, g, b, a);
			});
	}

	void Renderer::ClearMagenta()
	{
		Clear(1, 0, 1);
	}

	void Renderer::SetClearColor(float r, float g, float b, float a)
	{
	}

	void Renderer::DrawIndexed(uint32_t count, bool depthTest)
	{
		RM_RENDER_2(count, depthTest, {
			RendererAPI::DrawIndexed(count, depthTest);
			});
	}

	void Renderer::WaitAndRender()
	{
		s_Instance->m_CommandQueue.Execute();
	}


	//void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform)
	//{
	//	//shader->Bind();
	//	////std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMatrix4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);
	//	////std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMatrix4("u_Transform", transform);

	//	//vertexArray->Bind();
	//	//RenderCommand::DrawIndexed(vertexArray);
	//}
}