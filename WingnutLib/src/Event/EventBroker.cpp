#include "wingnut_pch.h"
#include "EventBroker.h"


#include "WindowEvents.h"


namespace Wingnut
{


	void EventBroker::ProcessEvent(Ref<Event> event)
	{
		switch (event->Type())
		{
		case EventType::WindowClosed:	EventCallback<WindowClosedEvent>::Execute(std::dynamic_pointer_cast<WindowClosedEvent>(event)); return;
		case EventType::WindowResized:	EventCallback<WindowResizedEvent>::Execute(std::dynamic_pointer_cast<WindowResizedEvent>(event)); return;
		}
	}



}
