#include "wingnut_pch.h"
#include "Camera.h"

#include "Event/EventUtils.h"
#include "Event/WindowEvents.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>


namespace Wingnut
{

	Ref<Camera> Camera::Create(const glm::vec3& position, uint32_t width, uint32_t height)
	{
		return CreateRef<Camera>(position, width, height);
	}


	Camera::Camera(const glm::vec3& position, uint32_t width, uint32_t height)
		: m_Position(position), m_Rotation(0.0f), m_LookAt(0.0f)
	{
		m_ProjectionMatrix = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 10000.0f);
		m_Distance = glm::distance(m_Position, m_LookAt);

		CalculateViewMatrix();

		SubscribeToEvent<WindowResizedEvent>([&](WindowResizedEvent& event)
			{
				if (event.Width() == 0 || event.Height() == 0)
					return false;

				m_ProjectionMatrix = glm::perspective(glm::radians(45.0f), (float)event.Width() / (float)event.Height(), 0.1f, 10000.0f);

				CalculateViewMatrix();

				return false;
			});
	}

	Camera::~Camera()
	{

	}

	void Camera::Update()
	{

	}

	void Camera::CalculateViewMatrix()
	{
		m_Position = CalculatePosition();
		m_Rotation = glm::eulerAngles(GetOrientation() * (180.0f / glm::pi<float>()));

		m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(GetOrientation());
		m_ViewMatrix = glm::inverse(m_ViewMatrix);
	}

	glm::vec3 Camera::CalculatePosition()
	{
		return m_LookAt - GetForwardDirection() * m_Distance;
	}

	glm::quat Camera::GetOrientation()
	{
		return glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
	}

	glm::vec3 Camera::GetForwardDirection()
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
	}

}
