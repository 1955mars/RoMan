#pragma once

#include "RoMan/Layer.h"
#include "RoMan/Events/ApplicationEvent.h"
#include "RoMan/Events/KeyEvent.h"
#include "RoMan/Events/MouseEvent.h"

namespace RoMan
{
	class ROMAN_API ImGuiLayer: public Layer
	{
	public:
		ImGuiLayer();
		ImGuiLayer(const std::string& name);
		~ImGuiLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

		void Begin();
		void End();

	private:
		float m_time = 0.0f;
	};
}



