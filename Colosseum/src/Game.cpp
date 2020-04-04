#include "rmpch.h"
#include <RoMan.h>

#include "imgui/imgui.h"
#include "glm/glm.hpp"

#include "glm/gtc/matrix_transform.hpp"


class ExampleLayer : public RoMan::Layer
{
public:
	ExampleLayer()
		: Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraPosition(0.0f)
	{
		m_VertexArray.reset(RoMan::VertexArray::Create());

		float vertices[3 * 7] =
		{
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			 0.5f, -0.5f, 0.0f,	0.2f, 0.3f, 0.8f, 1.0f,
			 0.0f,  0.5f, 0.0f,	0.8f, 0.8f, 0.2f, 1.0f
		};

		std::shared_ptr<RoMan::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(RoMan::VertexBuffer::Create(vertices, sizeof(vertices)));

		RoMan::BufferLayout layout = {
				{RoMan::ShaderDataType::Float3, "m_Position"},
				{RoMan::ShaderDataType::Float4, "m_Color"}
		};

		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		uint32_t indices[3] = { 0, 1, 2 };
		std::shared_ptr<RoMan::IndexBuffer> indexBuffer;
		indexBuffer.reset(RoMan::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		m_SquareVA.reset(RoMan::VertexArray::Create());
		float squareVertices[3 * 4] = {
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.5f,  0.5f, 0.0f,
			-0.5f,  0.5f, 0.0f
		};
		std::shared_ptr<RoMan::VertexBuffer> squareVB;
		squareVB.reset(RoMan::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		squareVB->SetLayout({
				{RoMan::ShaderDataType::Float3, "m_Posisition"}
			});

		m_SquareVA->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		std::shared_ptr<RoMan::IndexBuffer> squareIB;
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
		m_Shader.reset(new RoMan::Shader(vertexSrc, fragmentSrc));

		std::string blueShaderVertexSrc = R"(
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

		std::string blueShaderFragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec3 v_Position;

			void main()
			{
					color = vec4(0.2, 0.3, 0.8, 1.0);
			}
		)";

		m_BlueShader.reset(new RoMan::Shader(blueShaderVertexSrc, blueShaderFragmentSrc));
	}

	void OnUpdate(RoMan::Timestep ts) override
	{
		if (RoMan::Input::IsKeyPressed(RM_KEY_LEFT))
			m_CameraPosition.x -= m_CameraMoveSpeed * ts;
		else if (RoMan::Input::IsKeyPressed(RM_KEY_RIGHT))
			m_CameraPosition.x += m_CameraMoveSpeed * ts;

		if (RoMan::Input::IsKeyPressed(RM_KEY_UP))
			m_CameraPosition.y += m_CameraMoveSpeed * ts;
		else if (RoMan::Input::IsKeyPressed(RM_KEY_DOWN))
			m_CameraPosition.y -= m_CameraMoveSpeed * ts;

		if (RoMan::Input::IsKeyPressed(RM_KEY_Z))
			m_CameraRotation += m_CameraRotationspeed * ts;
		else if (RoMan::Input::IsKeyPressed(RM_KEY_X))
			m_CameraRotation -= m_CameraRotationspeed * ts;


		RoMan::RenderCommand::SetClearColor({ 0.1, 0.1, 0.1, 1 });
		RoMan::RenderCommand::Clear();

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);

		RoMan::Renderer::BeginScene(m_Camera);

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
		for (int x = 0; x < 20; x++)
		{
			for (int y = 0; y < 20; y++)
			{
				glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				RoMan::Renderer::Submit(m_BlueShader, m_SquareVA, transform);
			}
		}

		RoMan::Renderer::Submit(m_Shader, m_VertexArray);

		RoMan::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override
	{

	}

	void OnEvent(RoMan::Event& event) override
	{

	}

private:
	std::shared_ptr<RoMan::Shader> m_Shader;
	std::shared_ptr<RoMan::VertexArray> m_VertexArray;

	std::shared_ptr<RoMan::Shader> m_BlueShader;
	std::shared_ptr<RoMan::VertexArray> m_SquareVA;

	RoMan::OrthographicCamera m_Camera;
	glm::vec3 m_CameraPosition;
	float m_CameraMoveSpeed = 2.5f;

	float m_CameraRotation = 0.0f;
	float m_CameraRotationspeed = 10.0f;

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