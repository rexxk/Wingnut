#include "wingnut_pch.h"
#include "EventBroker.h"


#include "MouseEvents.h"
#include "WindowEvents.h"


namespace Wingnut
{


	void EventBroker::ProcessEvent(Ref<Event> event)
	{
		switch (event->Type())
		{
		case EventType::WindowClosed:	EventCallback<WindowClosedEvent>::Execute(std::dynamic_pointer_cast<WindowClosedEvent>(event)); return;
		case EventType::WindowResized:	EventCallback<WindowResizedEvent>::Execute(std::dynamic_pointer_cast<WindowResizedEvent>(event)); return;

		case EventType::MouseButtonPressed: EventCallback<MouseButtonPressedEvent>::Execute(std::dynamic_pointer_cast<MouseButtonPressedEvent>(event)); return;
		case EventType::MouseButtonReleased: EventCallback<MouseButtonReleasedEvent>::Execute(std::dynamic_pointer_cast<MouseButtonReleasedEvent>(event)); return;
		case EventType::MouseMoved: EventCallback<MouseMovedEvent>::Execute(std::dynamic_pointer_cast<MouseMovedEvent>(event)); return;
		case EventType::MouseWheel: EventCallback<MouseWheelEvent>::Execute(std::dynamic_pointer_cast<MouseWheelEvent>(event)); return;
		}
	}



}
