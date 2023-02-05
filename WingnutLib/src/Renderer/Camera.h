#pragma once

#include "Core/Timestep.h"

#include <glm/glm.hpp>


namespace Wingnut
{


	class Camera
	{
	public:
		static Ref<Camera> Create(const glm::vec3& position, uint32_t width, uint32_t height);

		Camera(const glm::vec3& position, uint32_t width, uint32_t height);
		~Camera();

		const glm::mat4&& GetViewProjectionMatrix() const { return m_ProjectionMatrix * m_ViewMatrix; }

		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }

		const glm::vec3& GetPosition() const { return m_Position; }

		void Update(Timestep ts);

	private:
		void CalculateViewMatrix();
		glm::vec3 CalculatePosition();

		glm::quat GetOrientation();

		glm::vec3 GetUpDirection();
		glm::vec3 GetRightDirection();
		glm::vec3 GetForwardDirection();

		float ZoomSpeed() const;

	private:

		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ProjectionMatrix;

		glm::vec3 m_Position;
		glm::vec3 m_Rotation;
		glm::vec3 m_LookAt;

		float m_Yaw = 0.0f;
		float m_Pitch = 0.0f;
		float m_Distance;

		float m_Exposure = 0.8f;

		float m_MouseSpeed = 1.0f;

	};


}
