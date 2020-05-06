#include "rmpch.h"
#include "Physics.h"
#include <RoMan.h>

#include "Platform/OpenGL/OpenGLShader.h"

#include "imgui/imgui.h"
#include "glm/glm.hpp"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

Scene scene(1.0f / 60.0f, 10);
Clock p_Clock;
glm::vec2 cameraBounds = { 80.0f, 40.0f };
bool frameStepping = false;
bool canStep = false;
bool printVert = true;

class ExampleLayer : public RoMan::Layer
{
public:
	ExampleLayer()
		: Layer("Example"), m_Camera(0, cameraBounds.x, 0, cameraBounds.y), m_CameraPosition(0.0f)
	{
		m_AddClock.Start();
		m_VertexArray.reset(RoMan::VertexArray::Create());

		// Spawn initial static objects
		Circle c(4.0f);
		Body* b = scene.Add(&c, cameraBounds.x / 2.0f, cameraBounds.y / 2.0f);
		b->SetStatic();

		PolygonShape poly;
		poly.SetBox(15.0f, 1.0f);
		b = scene.Add(&poly, cameraBounds.x / 2.0f, 5);
		b->SetStatic();
		b->SetOrient(0.0f);

		PolygonShape poly2;
		poly2.SetBox(5.0f, 3.0f);
		b = scene.Add(&poly2, 15, 10);
		b->SetStatic();
		b->SetOrient(0.0f);

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
	}

	void OnUpdate(RoMan::Timestep ts) override
	{
		/*if (RoMan::Input::IsKeyPressed(RM_KEY_LEFT))
			m_playerPosition.x -= m_PlayerMoveSpeed * ts;
		else if (RoMan::Input::IsKeyPressed(RM_KEY_RIGHT))
			m_playerPosition.x += m_PlayerMoveSpeed * ts;

		if (RoMan::Input::IsKeyPressed(RM_KEY_UP))
			m_playerPosition.y += m_PlayerMoveSpeed * ts;
		else if (RoMan::Input::IsKeyPressed(RM_KEY_DOWN))
			m_playerPosition.y -= m_PlayerMoveSpeed * ts;*/

		if (RoMan::Input::IsKeyPressed(RM_KEY_LEFT))
			m_CameraPosition.x -= m_CameraMoveSpeed * ts;
		else if (RoMan::Input::IsKeyPressed(RM_KEY_RIGHT))
			m_CameraPosition.x += m_CameraMoveSpeed * ts;

		if (RoMan::Input::IsKeyPressed(RM_KEY_UP))
			m_CameraPosition.y += m_CameraMoveSpeed * ts;
		else if (RoMan::Input::IsKeyPressed(RM_KEY_DOWN))
			m_CameraPosition.y -= m_CameraMoveSpeed * ts;

		if (RoMan::Input::IsMouseButtonPressed(RM_MOUSE_BUTTON_LEFT) && m_AddClock.Elapsed() > 0.5f)
		{
			m_AddClock.Start();
			auto [x, y] = RoMan::Input::GetMousePosition();
			auto width = RoMan::Application::Get().GetWindow().GetWidth();
			auto height = RoMan::Application::Get().GetWindow().GetHeight();

			x = (x / width) * cameraBounds.x;
			y = cameraBounds.y - (y / height) * cameraBounds.y;
			
			int xPos = x + (x < 0 ? -0.5 : 0.5);
			int yPos = y + (y < 0 ? -0.5 : 0.5);

			//RM_INFO("Spawned Pos = x: {0}, y: {1}", x, y);

			Circle c(Random(1.0f, 3.0f));
			Body* bCir = scene.Add(&c, xPos, yPos);
		}

		RoMan::RenderCommand::SetClearColor({ 0.1, 0.1, 0.1, 1 });
		RoMan::RenderCommand::Clear();

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);

		RoMan::Renderer::BeginScene(m_Camera);

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		// Physics loop
		//RM_INFO("Hello! Var = {0}", a);
		static float accumulator = 0;

		accumulator += p_Clock.Elapsed();

		p_Clock.Start();

		accumulator = glm::clamp(0.0f, 0.1f, accumulator);
		while (accumulator >= dt)
		{
			if (!frameStepping)
				scene.Step();
			else
			{
				if (canStep)
				{
					scene.Step();
					canStep = false;
				}
			}
			accumulator -= dt;
		}

		p_Clock.Stop();

		//Render loop
		std::dynamic_pointer_cast<RoMan::OpenGLShader>(m_FlatColorShader)->Bind();
		std::dynamic_pointer_cast<RoMan::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat3("u_Color", m_SquareColor);

		
		for (int i = 0; i < scene.bodies.size(); i++)
		{
			m_VertexArray.reset(RoMan::VertexArray::Create());

			Body* b = scene.bodies[i];
			b->shape->Draw();

			m_VertexArray.reset(RoMan::VertexArray::Create());
			
			RoMan::Ref<RoMan::VertexBuffer> vertexBuffer;

			if (b->shape->GetType() == Shape::eCircle)
			{
				float vertices[3 * 21];

				for (int i = 0; i < b->shape->vertexCount; i++)
				{
					vertices[i] = b->shape->vertices[i];
				}

				vertexBuffer.reset(RoMan::VertexBuffer::Create(vertices, sizeof(vertices)));
			}
			else
			{
				float vertices[3 * 4];

				for (int i = 0; i < b->shape->vertexCount; i++)
				{
					vertices[i] = b->shape->vertices[i];
				}

				vertexBuffer.reset(RoMan::VertexBuffer::Create(vertices, sizeof(vertices)));
			}

			//vertexBuffer.reset(RoMan::VertexBuffer::Create(b->shape->vertices, sizeof(float) * b->shape->vertexCount));

			//vertexBuffer.reset(RoMan::VertexBuffer::Create(vertices, sizeof(vertices)));

			vertexBuffer->SetLayout({
					{RoMan::ShaderDataType::Float3, "a_Position"}
				});

			m_VertexArray->AddVertexBuffer(vertexBuffer);

			RoMan::Ref<RoMan::IndexBuffer> indexBuffer;

			if (b->shape->GetType() == Shape::eCircle)
			{
				uint32_t indices[60] = { 0, 1, 2, 2, 3, 0,
									   0, 3, 4, 4, 5, 0,
									   0, 5, 6, 6, 7, 0,
									   0, 7, 8, 8, 9, 0,
									   0, 9, 10, 10, 11, 0,
									   0, 11, 12, 12, 13, 0,
									   0, 13, 14, 14, 15, 0,
									   0, 15, 16, 16, 17, 0,
									   0, 17, 18, 18, 19, 0,
									   0, 19, 20, 20, 1, 0 };

				indexBuffer.reset(RoMan::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
			}
			else
			{
				uint32_t indices[6] = { 0, 1, 2, 2, 3, 0 };
				indexBuffer.reset(RoMan::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
			}
			/*uint32_t indices[60] = { 0, 1, 2, 2, 3, 0,
									   0, 3, 4, 4, 5, 0,
									   0, 5, 6, 6, 7, 0,
									   0, 7, 8, 8, 9, 0,
									   0, 9, 10, 10, 11, 0,
									   0, 11, 12, 12, 13, 0,
									   0, 13, 14, 14, 15, 0,
									   0, 15, 16, 16, 17, 0,
									   0, 17, 18, 18, 19, 0,
									   0, 19, 20, 20, 1, 0 };*/

			//indexBuffer.reset(RoMan::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
			m_VertexArray->SetIndexBuffer(indexBuffer);

			//RM_INFO("Color = {0}, {1}, {2}", b->r, b->g, b->b);

			glm::vec3 color = { b->r, b->g, b->b };
			std::dynamic_pointer_cast<RoMan::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat3("u_Color", color);

			//RM_INFO("Body: {0} | Vel: {1}", i, b->velocity);

			glm::mat4 playerTransform = glm::translate(glm::mat4(1.0f), glm::vec3(b->position, 0.0f));
			RoMan::Renderer::Submit(m_FlatColorShader, m_VertexArray, playerTransform);
		}

		/*for (int x = 0; x < 20; x++)
		{
			for (int y = 0; y < 20; y++)
			{
				glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				RoMan::Renderer::Submit(m_FlatColorShader, m_SquareVA, transform);
			}
		}*/

		// My attemps to do things
		/*glm::vec3 playerColor = { 0.0f, 0.0f, 1.0f };
		glm::vec3 collisionColor = { 1.0f, 0.0f, 0.0f };

		std::dynamic_pointer_cast<RoMan::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat3("u_Color", playerColor);
		
		//glm::vec3 playerPos(0.0f, 0.0f, 0.0f);
		
		/*glm::mat4 tMatrix = { 1.0f, 0.0f, 0.0f, 1.0f,
							  0.0f, 1.0f, 0.0f, 1.0f,
							  0.0f, 0.0f, 1.0f, 0.0f,
							  0.0f, 0.0f, 0.0f, 1.0f };

		bool rectCollision = AABBvsAABB(&m_playerPosition, &m_rectPosition, &player, &rect);
		bool circleCollision = AABBvsCircle(&m_playerPosition, &m_circlePosition, &player, &circle);

		if (rectCollision || circleCollision)
			std::dynamic_pointer_cast<RoMan::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat3("u_Color", collisionColor);

		glm::mat4 playerTransform = glm::translate(glm::mat4(1.0f), m_playerPosition);
		RoMan::Renderer::Submit(m_FlatColorShader, m_PlayerVA, playerTransform);

		if (!rectCollision)
			std::dynamic_pointer_cast<RoMan::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat3("u_Color", playerColor);
		else
			std::dynamic_pointer_cast<RoMan::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat3("u_Color", collisionColor);

		glm::mat4 rectTransform = glm::translate(glm::mat4(1.0f), m_rectPosition);
		RoMan::Renderer::Submit(m_FlatColorShader, m_SquareVA, rectTransform);

		if (!circleCollision)
			std::dynamic_pointer_cast<RoMan::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat3("u_Color", playerColor);
		else
			std::dynamic_pointer_cast<RoMan::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat3("u_Color", collisionColor);

		glm::mat4 circleTransform = glm::translate(glm::mat4(1.0f), m_circlePosition);
		RoMan::Renderer::Submit(m_FlatColorShader, m_CircleVA, circleTransform);*/
		// End attempt

		/*auto textureShader = m_ShaderLibrary.Get("Texture");

		m_Texture->Bind();
		RoMan::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

		m_RITlogoTexture->Bind();
		RoMan::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

		//Triangle
		RoMan::Renderer::Submit(m_Shader, m_VertexArray);*/

		RoMan::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override
	{
		//ImGui::Begin("Settings");
		//ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
		//ImGui::End();
	}

	void OnEvent(RoMan::Event& event) override
	{

	}

private:
	RoMan::ShaderLibrary m_ShaderLibrary;

	RoMan::Ref<RoMan::Shader> m_Shader;
	RoMan::Ref<RoMan::VertexArray> m_VertexArray;

	RoMan::Ref<RoMan::Shader> m_FlatColorShader;
	RoMan::Ref<RoMan::VertexArray> m_SquareVA;

	// Player stuff
	Clock m_AddClock;
	/*Box player = Box(0.3f, 0.3f);
	RoMan::Ref<RoMan::VertexArray> m_PlayerVA;
	glm::vec3 m_playerPosition = { 0.0f, 0.0f, 0.0f };
	float m_PlayerMoveSpeed = 2.0f;
	Box rect = Box(0.5f, 0.25f);
	glm::vec3 m_rectPosition = { -0.5f, 0.1f, 0.0f };

	Circle circle = Circle(0.25f);
	RoMan::Ref<RoMan::VertexArray> m_CircleVA;
	glm::vec3 m_circlePosition = { 0.75f, -0.6f, 0.0f };*/

	// End stuff
	RoMan::Ref<RoMan::Texture2D> m_Texture, m_RITlogoTexture;

	RoMan::OrthographicCamera m_Camera;
	glm::vec3 m_CameraPosition;
	float m_CameraMoveSpeed = 10.0f;

	float m_CameraRotation = 0.0f;
	float m_CameraRotationspeed = 10.0f;

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