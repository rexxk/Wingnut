#include "wingnut_pch.h"
#include "KeyboardInput.h"

#include "Event/EventUtils.h"
#include "Event/KeyboardEvents.h"


namespace Wingnut
{

	KeyboardInput::KeyboardInput()
	{
		if (s_Instance == nullptr)
		{
			s_Instance = this;
		}

		SubscribeToEvent<KeyPressedEvent>([&](KeyPressedEvent& event)
			{
				m_KeyStates[event.Key()] = true;

				return false;
			});

		SubscribeToEvent<KeyReleasedEvent>([&](KeyReleasedEvent& event)
			{
				m_KeyStates[event.Key()] = false;

				return false;
			});
	}

}
