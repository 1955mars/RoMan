#pragma once

#include "Core.h"

#include "Window.h"
#include "RoMan/LayerStack.h"
#include "Events/Event.h"
#include "RoMan/Events/ApplicationEvent.h"

#include "RoMan/ImGui/ImGuiLayer.h"

#include "RoMan/Renderer/Shader.h"

#include "RoMan/Renderer/Buffer.h"

#include "RoMan/Renderer/VertexArray.h"

#include "RoMan/Renderer/OrthographicCamera.h"

namespace RoMan
{
	struct ApplicationProps
	{
		std::string Name;
		uint32_t WindowWidth, WindowHeight;
	};

	class ROMAN_API Application
	{
	public:
		Application(const ApplicationProps& props = { "RoMan Engine", 1280, 720 });
		virtual ~Application();
		void Run();

		virtual void OnInit() {}
		virtual void OnShutDown() {}
		virtual void OnUpdate(Timestep ts) {}

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
		void RenderImGui();

		std::string OpenFile(const std::string& filter) const;

		inline Window& GetWindow() { return *m_Window; }

		inline static Application& Get() { return *s_Instance; }

	private:
		bool OnWindowResize(WindowResizeEvent& e);
		bool OnWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
		LayerStack m_LayerStack;
		ImGuiLayer* m_ImGuiLayer;
		Timestep m_TimeStep;

		float m_LastFrameTime = 0.0f;
	private:
		static Application* s_Instance;
	};

	//To be defined in client
	Application* CreateApplication();
}



