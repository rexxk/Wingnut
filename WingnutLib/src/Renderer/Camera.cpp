#include "wingnut_pch.h"
#include "Camera.h"

#include "Event/EventUtils.h"
#include "Event/UIEvents.h"
#include "Event/WindowEvents.h"

#include "Input/MouseInput.h"

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
		m_ProjectionMatrix = glm::perspectiveFovLH(glm::radians(45.0f), (float)width, (float)height, 0.1f, 10000.0f);
//		m_ProjectionMatrix = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 10000.0f);
		m_Distance = glm::distance(m_Position, m_LookAt);

		m_Yaw = glm::pi<float>();
		m_Pitch = glm::pi<float>();

		CalculateViewMatrix();

//		SubscribeToEvent<WindowResizedEvent>([&](WindowResizedEvent& event)
		SubscribeToEvent<UIViewportResizedEvent>([&](UIViewportResizedEvent& event)
			{
				if (event.Width() == 0 || event.Height() == 0)
					return false;

				m_ProjectionMatrix = glm::perspectiveFovLH(glm::radians(45.0f), (float)event.Width(), (float)event.Height(), 0.1f, 10000.0f);
//				m_ProjectionMatrix = glm::perspective(glm::radians(45.0f), (float)event.Width() / (float)event.Height(), 0.1f, 10000.0f);

				CalculateViewMatrix();

				return false;
			});
	}

	Camera::~Camera()
	{

	}

	void Camera::Update(Timestep ts)
	{
		if (!m_Enabled)
		{
			return;
		}

		if (MouseInput::IsButtonPressed(MouseButton::Right))
		{
//			LOG_CORE_TRACE("[Right button] - Position: {},{} - Delta: {},{}", MouseInput::PositionX(), MouseInput::PositionY(), MouseInput::DeltaX(), MouseInput::DeltaY());

			float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
			m_Yaw -= yawSign * MouseInput::DeltaX() * (m_MouseSpeed /* * 10 */) * ts;
			m_Pitch += MouseInput::DeltaY() * (m_MouseSpeed /* * 10 */) * ts;

		}
		else if (MouseInput::IsButtonPressed(MouseButton::Middle))
		{
			m_LookAt += -GetRightDirection() * (float)MouseInput::DeltaX() * m_MouseSpeed * m_Distance * (float)ts;
			m_LookAt -= GetUpDirection() * (float)MouseInput::DeltaY() * m_MouseSpeed * m_Distance * (float)ts;
		}

		int32_t wheelDelta = MouseInput::WheelDelta();
		if (wheelDelta != 0)
		{
			m_Distance -= wheelDelta * ZoomSpeed();

			if (m_Distance < 1.0f)
			{
				m_Distance = 1.0f;
			}
		}

		CalculateViewMatrix();

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
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, 1.0f));
	}

	glm::vec3 Camera::GetUpDirection()
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::vec3 Camera::GetRightDirection()
	{
		return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	float Camera::ZoomSpeed() const
	{
		float distance = m_Distance * 0.2f;
		distance = std::max(distance, 0.0f);

		float speed = distance * distance;
		speed = std::min(speed, 100.0f);

		return speed;
	}

}
