#pragma once

#include <chrono>


namespace Wingnut
{

	class Timestep
	{
	public:
		Timestep()
			: m_Timestep(0.0f)
		{
			m_StartTime = std::chrono::steady_clock::now();
		}

		void Update()
		{
			std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
			
			m_Timestep = (float)std::chrono::duration_cast<std::chrono::microseconds>(t2 - m_StartTime).count() * 0.001f * 0.001f;

			m_StartTime = t2;
		}

		operator float() const { return m_Timestep; }

	private:
		std::chrono::steady_clock::time_point m_StartTime;

		float m_Timestep;
	};


}
