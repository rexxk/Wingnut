#pragma once

#include "Event.h"

#include "Core/Application.h"



void AddEventToQueue(Ref<Wingnut::Event> event);
void ExecuteEvent(Ref<Wingnut::Event> event);


template<typename T, typename Fn>
void SubscribeToEvent(Fn fn)
{
	Wingnut::Application::Get().GetEventBroker()->SubscribeToEvent<T>(fn);
}
