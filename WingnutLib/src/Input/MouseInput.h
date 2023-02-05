#pragma once

#include "Input/KeyDefs.h"


namespace Wingnut
{


	class MouseInput
	{
	public:
		MouseInput();


		static int32_t DeltaX();
		static int32_t DeltaY();

		static uint32_t PositionX() { return s_Instance->m_MousePositionX; }
		static uint32_t PositionY() { return s_Instance->m_MousePositionY; }

		static int32_t WheelDelta();

		static bool IsButtonPressed(MouseButton button) { return s_Instance->m_ButtonStates[button]; }

	private:
		inline static MouseInput* s_Instance = nullptr;

		uint32_t m_MousePositionX = 0;
		uint32_t m_MousePositionY = 0;

		int32_t m_MouseDeltaX = 0;
		int32_t m_MouseDeltaY = 0;

		int32_t m_WheelDelta = 0;

		std::unordered_map<MouseButton, bool> m_ButtonStates;
	};



}
