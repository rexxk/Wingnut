#pragma once




namespace Wingnut
{


	enum class EventType
	{
		WindowClosed, WindowResized,
	};



	class Event
	{
	public:
		Event(EventType eventType)
			: m_EventType(eventType)
		{

		}

		virtual ~Event()
		{

		}


		EventType Type() const { return m_EventType; }

		bool Handled = false;

	private:
		EventType m_EventType;

	};



}
