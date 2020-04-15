#include "rmpch.h"
#include <RoMan.h>

#include "Platform/OpenGL/OpenGLShader.h"

#include "imgui/imgui.h"
#include "glm/glm.hpp"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

class ExampleLayer : public RoMan::Layer
{
public:
	ExampleLayer()
		: Layer("Example"), m_Camera(glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.1f, 10000.0f))
	{
		m_VertexArray.reset(RoMan::VertexArray::Create());

		float vertices[3 * 7] =
		{
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			 0.5f, -0.5f, 0.0f,	0.2f, 0.3f, 0.8f, 1.0f,
			 0.0f,  0.5f, 0.0f,	0.8f, 0.8f, 0.2f, 1.0f
		};

		RoMan::Ref<RoMan::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(RoMan::VertexBuffer::Create(vertices, sizeof(vertices)));

		RoMan::BufferLayout layout = {
				{RoMan::ShaderDataType::Float3, "m_Position"},
				{RoMan::ShaderDataType::Float4, "m_Color"}
		};

		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		uint32_t indices[3] = { 0, 1, 2 };
		RoMan::Ref<RoMan::IndexBuffer> indexBuffer;
		indexBuffer.reset(RoMan::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		m_SquareVA.reset(RoMan::VertexArray::Create());
		float squareVertices[5 * 4] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};
		RoMan::Ref<RoMan::VertexBuffer> squareVB;
		squareVB.reset(RoMan::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		squareVB->SetLayout({
				{RoMan::ShaderDataType::Float3, "a_Position"},
				{RoMan::ShaderDataType::Float2, "a_TexCoord"}
			});

		m_SquareVA->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		RoMan::Ref<RoMan::IndexBuffer> squareIB;
		squareIB.reset(RoMan::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareVA->SetIndexBuffer(squareIB);

		std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);	
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_Position;
			in vec4 v_Color;

			void main()
			{
				color = vec4(v_Position * 0.5 + 0.5, 1.0);
				color = v_Color;
			}
		)";
		m_Shader = RoMan::Shader::Create("VertexPosColr", vertexSrc, fragmentSrc);

		std::string flatColorShaderVertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;


			void main()
			{
					v_Position = a_Position;
					gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}
		)";

		std::string flatColorShaderFragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec3 v_Position;

			uniform vec3 u_Color;

			void main()
			{
					color = vec4(u_Color, 1.0);
			}
		)";

		m_FlatColorShader = RoMan::Shader::Create("FlatColor", flatColorShaderVertexSrc, flatColorShaderFragmentSrc);

		auto textureShader = m_ShaderLibrary.Load("assets/shaders/Texture.glsl");

		m_Texture = RoMan::Texture2D::Create("assets/textures/Checkerboard.png");
		m_RITlogoTexture = RoMan::Texture2D::Create("assets/textures/RITlogo.png");

		std::dynamic_pointer_cast<RoMan::OpenGLShader>(textureShader)->Bind();
		std::dynamic_pointer_cast<RoMan::OpenGLShader>(textureShader)->UploadUniformInt("u_Texture", 0);

		m_SphereMesh.reset(new RoMan::Mesh("assets/meshes/Sphere1m.fbx"));
	}

	void OnUpdate(RoMan::Timestep ts) override
	{
		//if (RoMan::Input::IsKeyPressed(RM_KEY_LEFT))
		//	m_CameraPosition.x -= m_CameraMoveSpeed * ts;
		//else if (RoMan::Input::IsKeyPressed(RM_KEY_RIGHT))
		//	m_CameraPosition.x += m_CameraMoveSpeed * ts;

		//if (RoMan::Input::IsKeyPressed(RM_KEY_UP))
		//	m_CameraPosition.y += m_CameraMoveSpeed * ts;
		//else if (RoMan::Input::IsKeyPressed(RM_KEY_DOWN))
		//	m_CameraPosition.y -= m_CameraMoveSpeed * ts;

		//if (RoMan::Input::IsKeyPressed(RM_KEY_Z))
		//	m_CameraRotation += m_CameraRotationspeed * ts;
		//else if (RoMan::Input::IsKeyPressed(RM_KEY_X))
		//	m_CameraRotation -= m_CameraRotationspeed * ts;


		RoMan::RenderCommand::SetClearColor({ 0.9, 0.9, 0.9, 1 });
		RoMan::RenderCommand::Clear();

		//m_CameraOrtho.SetPosition(m_CameraPosition);
		//m_CameraOrtho.SetRotation(m_CameraRotation);

		m_Camera.Update();

		RoMan::Renderer::BeginScene(m_Camera);

		//glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		std::dynamic_pointer_cast<RoMan::OpenGLShader>(m_FlatColorShader)->Bind();
		std::dynamic_pointer_cast<RoMan::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat3("u_Color", m_SquareColor);

		//for (int x = 0; x < 20; x++)
		//{
		//	for (int y = 0; y < 20; y++)
		//	{
		//		glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
		//		glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
		//		RoMan::Renderer::Submit(m_FlatColorShader, m_SquareVA, transform);
		//	}
		//}

		auto textureShader = m_ShaderLibrary.Get("Texture");

		//m_Texture->Bind();
		//RoMan::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

		//m_RITlogoTexture->Bind();
		//RoMan::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

		//Triangle
		//RoMan::Renderer::Submit(m_Shader, m_VertexArray);
		RoMan::Renderer::Submit(textureShader, m_SphereMesh->GetVertexArray());

		RoMan::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
		ImGui::End();
	}

	void OnEvent(RoMan::Event& event) override
	{

	}

private:
	RoMan::ShaderLibrary m_ShaderLibrary;

	RoMan::Ref<RoMan::Mesh> m_SphereMesh;

	RoMan::Ref<RoMan::Shader> m_Shader;
	RoMan::Ref<RoMan::VertexArray> m_VertexArray;

	RoMan::Ref<RoMan::Shader> m_FlatColorShader;
	RoMan::Ref<RoMan::VertexArray> m_SquareVA;

	RoMan::Ref<RoMan::Texture2D> m_Texture, m_RITlogoTexture;

	RoMan::Camera m_Camera;
	//RoMan::OrthographicCamera m_CameraOrtho;

	//glm::vec3 m_CameraPosition;
	//float m_CameraMoveSpeed = 2.5f;

	//float m_CameraRotation = 0.0f;
	//float m_CameraRotationspeed = 10.0f;

	glm::vec3 m_SquareColor = { 0.2f, 0.3f, 0.8f };
};


class Colosseum : public RoMan::Application
{
public:
	Colosseum()
	{
		PushLayer(new ExampleLayer());
	}

	~Colosseum()
	{

	}
};

RoMan::Application* RoMan::CreateApplication()
{
	return new Colosseum();
}