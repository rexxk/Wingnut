#pragma once

#include "Event.h"



namespace Wingnut
{



	template<typename T>
	class EventCallback
	{
	public:
		template<typename Fn>
		static void AddCallback(Fn fn)
		{
			if (s_Instance == nullptr)
				s_Instance = new EventCallback<T>();

			s_Instance->m_Callbacks.emplace_back(fn);
		}

		size_t CallbackCount() { return s_Instance ? s_Instance->m_Callbacks.size() : 0; }

		static bool Execute(Ref<T> event)
		{
			if (s_Instance)
			{
				for (auto& callback : s_Instance->m_Callbacks)
				{
					event->Handled = callback(*(T*&)event);

					if (event->Handled)
						return true;
				}
			}

			return false;
		}

	private:
		inline static EventCallback<T>* s_Instance = nullptr;

		std::vector<std::function<bool(T&)>> m_Callbacks;
	};



	class EventBroker
	{
	public:
		EventBroker() = default;

		template<typename T>
		void SubscribeToEvent(std::function<bool(T&)> fn)
		{
			EventCallback<T>::AddCallback(fn);
		}

		static void ProcessEvent(Ref<Event> event);


	private:



	};


}
