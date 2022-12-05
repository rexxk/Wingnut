#include "wingnut_pch.h"
#include "EventUtils.h"

#include "Core/Application.h"



void AddEventToQueue(Ref<Wingnut::Event> event)
{

	Wingnut::Application::Get().GetEventQueue()->Add(event);

}

void ExecuteEvent(Ref<Wingnut::Event> event)
{
	Wingnut::Application::Get().GetEventBroker()->ProcessEvent(event);
}
