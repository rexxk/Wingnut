#pragma once

#include "Base.h"

#include <spdlog/spdlog.h>


namespace Wingnut
{


	class Log
	{
	public:
		static void Init();

		static spdlog::logger& CoreLogger() { return *s_CoreLogger; }
		static spdlog::logger& ClientLogger() { return *s_ClientLogger; }

	private:
		inline static Ref<spdlog::logger> s_CoreLogger = nullptr;
		inline static Ref<spdlog::logger> s_ClientLogger = nullptr;

	};


#define LOG_CORE_TRACE(msg, ...) Wingnut::Log::CoreLogger().trace(msg, __VA_ARGS__);
#define LOG_CORE_WARN(msg, ...) Wingnut::Log::CoreLogger().warn(msg, __VA_ARGS__);
#define LOG_CORE_ERROR(msg, ...) Wingnut::Log::CoreLogger().error(msg, __VA_ARGS__);
#define LOG_CORE_CRITICAL(msg, ...) Wingnut::Log::CoreLogger().critical(msg, __VA_ARGS__);

#define LOG_TRACE(msg, ...) Wingnut::Log::ClientLogger().trace(msg, __VA_ARGS__);
#define LOG_WARN(msg, ...) Wingnut::Log::ClientLogger().warn(msg, __VA_ARGS__);
#define LOG_ERROR(msg, ...) Wingnut::Log::ClientLogger().error(msg, __VA_ARGS__);
#define LOG_CRITICAL(msg, ...) Wingnut::Log::ClientLogger().critical(msg, __VA_ARGS__);


#if _DEBUG
	#define CORE_ASSERT(condition, msg, ...) if (!condition) { LOG_CORE_ERROR(msg, __VA_ARGS__); __debugbreak(); }
#else
	#define CORE_ASSERT(condition, msg, ...)
#endif



}
