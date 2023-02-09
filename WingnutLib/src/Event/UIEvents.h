#pragma once

#include "Event.h"

#include "Scene/Entity.h"

#include <cstdint>


namespace Wingnut
{


	class UIViewportResizedEvent : public Event
	{
	public:
		UIViewportResizedEvent(uint32_t width, uint32_t height)
			: Event(EventType::UIViewportResized), m_Width(width), m_Height(height)
		{

		}

		uint32_t Width() const { return m_Width; }
		uint32_t Height() const { return m_Height; }

	private:
		uint32_t m_Width;
		uint32_t m_Height;

	};

	class EntitySelectedEvent : public Event
	{
	public:
		EntitySelectedEvent(const Entity& entity)
			: Event(EventType::EntitySelected), m_Entity(entity)
		{

		}

		Entity& GetEntity() { return m_Entity; }

	private:
		Entity m_Entity;
	};

}
