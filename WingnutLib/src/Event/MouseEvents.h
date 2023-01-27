#pragma once

#include "Event.h"

#include "Input/KeyDefs.h"

#include <cstdint>


namespace Wingnut
{


	class MouseButtonPressedEvent : public Event
	{
	public:
		MouseButtonPressedEvent(MouseButton mouseButton, uint32_t positionX, uint32_t positionY)
			: Event(EventType::MouseButtonPressed), m_MouseButton(mouseButton), m_PositionX(positionX), m_PositionY(positionY)
		{

		}

		MouseButton GetMouseButton() const { return m_MouseButton; }

		uint32_t PositionX() const { return m_PositionX; }
		uint32_t PositionY() const { return m_PositionY; }
		
	private:
		MouseButton m_MouseButton;

		uint32_t m_PositionX;
		uint32_t m_PositionY;
	};

	
	class MouseButtonReleasedEvent : public Event
	{
	public:
		MouseButtonReleasedEvent(MouseButton mouseButton, uint32_t positionX, uint32_t positionY)
			: Event(EventType::MouseButtonReleased), m_MouseButton(mouseButton), m_PositionX(positionX), m_PositionY(positionY)
		{

		}

		MouseButton GetMouseButton() const { return m_MouseButton; }

		uint32_t PositionX() const { return m_PositionX; }
		uint32_t PositionY() const { return m_PositionY; }

	private:
		MouseButton m_MouseButton;

		uint32_t m_PositionX;
		uint32_t m_PositionY;
	};


	class MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(uint32_t positionX, uint32_t positionY)
			: Event(EventType::MouseMoved), m_PositionX(positionX), m_PositionY(positionY)
		{

		}

		uint32_t PositionX() const { return m_PositionX; }
		uint32_t PositionY() const { return m_PositionY; }

	private:
		uint32_t m_PositionX;
		uint32_t m_PositionY;
	};

	
	class MouseWheelEvent : public Event
	{
	public:
		MouseWheelEvent(int32_t delta, uint32_t positionX, uint32_t positionY)
			: Event(EventType::MouseWheel), m_Delta(delta), m_PositionX(positionX), m_PositionY(positionY)
		{

		}

		int32_t Delta() const { return m_Delta; }

		uint32_t PositionX() const { return m_PositionX; }
		uint32_t PositionY() const { return m_PositionY; }

	private:
		int32_t m_Delta;

		uint32_t m_PositionX;
		uint32_t m_PositionY;
	};

}
