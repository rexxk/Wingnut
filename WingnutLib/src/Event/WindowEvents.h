#pragma once


#include "Event.h"



namespace Wingnut
{


	class WindowClosedEvent : public Event
	{
	public:
		WindowClosedEvent()
			: Event(EventType::WindowClosed)
		{

		}

	};


	class WindowResizedEvent : public Event
	{
	public:
		WindowResizedEvent(uint32_t width, uint32_t height)
			: Event(EventType::WindowResized), m_Width(width), m_Height(height)
		{

		}

		uint32_t Width() const { return m_Width; }
		uint32_t Height() const { return m_Height; }

	private:
		uint32_t m_Width;
		uint32_t m_Height;
	};


	class RendererCompletedEvent : public Event
	{
	public:
		RendererCompletedEvent()
			: Event(EventType::RendererCompleted)
		{

		}

	};

}
