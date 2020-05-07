#include "rmpch.h"
#include "Physics.h"
#include <RoMan.h>

#include "Platform/OpenGL/OpenGLShader.h"

#include "imgui/imgui.h"
#include "glm/glm.hpp"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

glm::vec2 cameraBounds = { 80.0f, 40.0f };

Scene scene(1.0f / 60.0f, 10); // Physics scene
Clock p_Clock; // Physics clock
bool frameStepping = false;
bool canStep = false;

class ExampleLayer : public RoMan::Layer
{
public:
	ExampleLayer()
		: Layer("Example"), m_Camera(0, cameraBounds.x, 0, cameraBounds.y), m_CameraPosition(0.0f)
	{
		m_AddClock.Start(); // Start clock for spawning objects
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
		b = scene.Add(&poly2, 10, 10);
		b->SetStatic();
		b->SetOrient(0.0f);

		// Define vertex and fragment components of shader
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
		// Spawn a circle at the mouse position
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

			Circle c(Random(1.0f, 3.0f));
			Body* bCir = scene.Add(&c, xPos, yPos);
		}

		RoMan::RenderCommand::SetClearColor({ 0.1, 0.1, 0.1, 1 });
		RoMan::RenderCommand::Clear();

		m_Camera.SetPosition(m_CameraPosition);

		RoMan::Renderer::BeginScene(m_Camera);

		// Physics loop
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
		
		for (int i = 0; i < scene.bodies.size(); i++)
		{
			m_VertexArray.reset(RoMan::VertexArray::Create());

			Body* b = scene.bodies[i];
			b->shape->Draw(); // Update vertex positions

			// Create vertex array
			m_VertexArray.reset(RoMan::VertexArray::Create());
			
			RoMan::Ref<RoMan::VertexBuffer> vertexBuffer;

			vertexBuffer.reset(RoMan::VertexBuffer::Create(b->shape->vertices, sizeof(float) * b->shape->vertexCount));

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
			m_VertexArray->SetIndexBuffer(indexBuffer);

			// Set color
			glm::vec3 color = { b->r, b->g, b->b };
			std::dynamic_pointer_cast<RoMan::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat3("u_Color", color);
			
			// Render shape
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(b->position, 0.0f));
			RoMan::Renderer::Submit(m_FlatColorShader, m_VertexArray, transform);
		}

		RoMan::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override
	{
	
	}

	void OnEvent(RoMan::Event& event) override
	{
	
	}

private:
	RoMan::Ref<RoMan::VertexArray> m_VertexArray;

	RoMan::Ref<RoMan::Shader> m_FlatColorShader;

	Clock m_AddClock;

	RoMan::OrthographicCamera m_Camera;
	glm::vec3 m_CameraPosition;
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