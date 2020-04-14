#pragma once

#include <glm/glm.hpp>

namespace RoMan
{
	class Camera
	{
	public:
		Camera(const glm::mat4& projectionMatrix);

		void Focus();
		void Update();

		inline float GetDistance() const { return m_Distance; }
		inline float SetDistance(float distance) { m_Distance = distance; }

		inline void SetProjectionMatrix(glm::mat4& projectionMatrix) { m_ProjectionMatrix = projectionMatrix; }

		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

		glm::vec3 GetUpDirection();
		glm::vec3 GetRightDirection();
		glm::vec3 GetForwardDirection();

		const glm::vec3& GetPosition() const { return m_Position; }

	private:
		void MousePan(const glm::vec2& delta);
		void MouseRotate(const glm::vec2& delta);
		void MouseZoom(float delta);

		glm::vec3 CalculatePosition();
		glm::quat GetOrientation();

	private:
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ViewProjectionMatrix;

		glm::vec3 m_Position;
		glm::vec3 m_Rotation;
		glm::vec3 m_FocalPoint;

		bool m_Panning;
		bool m_Rotating;

		glm::vec2 m_InitialMousePosition;
		glm::vec3 m_InitialFocalPoint;
		glm::vec3 m_InitialRotation;

		float m_Distance;
		float m_PanSpeed, m_RotationSpeed, m_ZoomSpeed;

		float m_Pitch, m_Yaw;
	};
}