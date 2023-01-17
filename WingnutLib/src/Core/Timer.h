#pragma once

#include <chrono>


namespace Wingnut
{

	class Timer
	{
	public:
		Timer()
		{
			Reset();
		}

		void Reset()
		{
			m_StartTime = std::chrono::steady_clock::now();
		}

		float ElapsedTime()
		{
			std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();

			return (float)std::chrono::duration_cast<std::chrono::milliseconds>(t2 - m_StartTime).count() / 1000.0 * 1000.0;
		}

	private:
		std::chrono::steady_clock::time_point m_StartTime;
	};


}
