#include "wingnut_pch.h"
#include "MouseInput.h"

#include "Event/EventUtils.h"
#include "Event/MouseEvents.h"


namespace Wingnut
{


	MouseInput::MouseInput()
	{
		if (s_Instance == nullptr)
		{
			s_Instance = this;


			m_ButtonStates[MouseButton::Left] = false;
			m_ButtonStates[MouseButton::Middle] = false;
			m_ButtonStates[MouseButton::Right] = false;
		}


		SubscribeToEvent<MouseMovedEvent>([&](MouseMovedEvent& event)
			{
				m_MouseDeltaX = (int32_t)event.PositionX() - (int32_t)m_MousePositionX;
				m_MouseDeltaY = (int32_t)event.PositionY() - (int32_t)m_MousePositionY;

				m_MousePositionX = event.PositionX();
				m_MousePositionY = event.PositionY();

				return false;
			});

		SubscribeToEvent<MouseWheelEvent>([&](MouseWheelEvent& event)
			{
				m_WheelDelta += event.Delta();

				return false;
			});

		SubscribeToEvent<MouseButtonPressedEvent>([&](MouseButtonPressedEvent& event)
			{
				m_MouseDeltaX = (int32_t)event.PositionX() - (int32_t)m_MousePositionX;
				m_MouseDeltaY = (int32_t)event.PositionY() - (int32_t)m_MousePositionY;

				m_ButtonStates[event.GetMouseButton()] = true;

				return false;
			});

		SubscribeToEvent<MouseButtonReleasedEvent>([&](MouseButtonReleasedEvent& event)
			{
				m_MouseDeltaX = (int32_t)event.PositionX() - (int32_t)m_MousePositionX;
				m_MouseDeltaY = (int32_t)event.PositionY() - (int32_t)m_MousePositionY;

				m_ButtonStates[event.GetMouseButton()] = false;

				return false;
			});
	}


	int32_t MouseInput::DeltaX()
	{ 
		int32_t returnValue = s_Instance->m_MouseDeltaX;
		s_Instance->m_MouseDeltaX = 0;

		return returnValue;
	}

	int32_t MouseInput::DeltaY() 
	{ 
		int32_t returnValue = s_Instance->m_MouseDeltaY;
		s_Instance->m_MouseDeltaY = 0;

		return returnValue;
	}

	int32_t MouseInput::WheelDelta()
	{
		int32_t returnValue = s_Instance->m_WheelDelta;
		s_Instance->m_WheelDelta = 0;

		return returnValue;
	}

}
