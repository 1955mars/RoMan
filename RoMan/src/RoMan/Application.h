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
	class ROMAN_API Application
	{
	public:
		Application();
		virtual ~Application() = default;
		void Run();

		void OnEvent(Event& e);
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline Window& GetWindow() { return *m_Window; }

		inline static Application& Get() { return *s_Instance; }

	private:
		bool OnWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		LayerStack m_LayerStack;

		float m_LastFrameTime = 0.0f;
	private:
		static Application* s_Instance;
	};

	//To be defined in client
	Application* CreateApplication();
}



