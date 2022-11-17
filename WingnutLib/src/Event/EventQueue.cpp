#include "wingnut_pch.h"
#include "EventQueue.h"

#include "EventBroker.h"



namespace Wingnut
{

	void EventQueue::Add(Ref<Event> event)
	{
		m_Queue.emplace(event);
	}

	void EventQueue::Flush()
	{
		while (m_Queue.size() > 0)
		{
			m_Queue.pop();
		}
	}

	void EventQueue::Process()
	{
		while (m_Queue.size() > 0)
		{
			Ref<Event> event = m_Queue.back();
			m_Queue.pop();

			EventBroker::ProcessEvent(event);
		}
	}


}
