#pragma once


namespace Wingnut
{

	class KeyboardInput
	{
	public:
		KeyboardInput();

		static bool IsKeyPressed(uint32_t key) { return s_Instance->m_KeyStates[key]; }

	private:
		inline static KeyboardInput* s_Instance = nullptr;

		std::unordered_map<uint32_t, bool> m_KeyStates;
	};


}
