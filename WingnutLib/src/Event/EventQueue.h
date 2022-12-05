#pragma once

#include "Event.h"


namespace Wingnut
{


	class EventQueue
	{
	public:
		EventQueue() = default;

		void Add(Ref<Event> event);
		void Flush();

		void Process();

	private:
		std::queue<Ref<Event>> m_Queue;
	};



}
