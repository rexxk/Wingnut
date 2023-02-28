#pragma once

#include "Event.h"

#include <cstdint>


namespace Wingnut
{

	class KeyPressedEvent : public Event
	{
	public:
		KeyPressedEvent(uint32_t scancode, uint32_t repeat)
			: Event(EventType::KeyPressed), m_Scancode(scancode), m_Repeat(repeat)
		{
			LOG_CORE_TRACE("KeyPressedEvent");
		}

		uint32_t Key() const { return m_Scancode; }
		uint32_t Repeat() const { return m_Repeat; }

	private:
		uint32_t m_Scancode;
		uint32_t m_Repeat;

	};


	class KeyReleasedEvent : public Event
	{
	public:
		KeyReleasedEvent(uint32_t scancode)
			: Event(EventType::KeyReleased), m_Scancode(scancode)
		{

		}

		uint32_t Key() const { return m_Scancode; }

	private:
		uint32_t m_Scancode;
	};


	class KeyTypedEvent : public Event
	{
	public:
		KeyTypedEvent(uint32_t scancode)
			: Event(EventType::KeyTyped), m_Scancode(scancode)
		{
			LOG_CORE_TRACE("KeyTypedEvent");
		}

		uint32_t Key() const { return m_Scancode; }

	private:
		uint32_t m_Scancode;

	};

}
