#pragma once

#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace RoMan
{
	class ROMAN_API Log
	{
	public: 
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

// Core log macros

#define RM_CORE_TRACE(...)    RoMan::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define RM_CORE_INFO(...)     RoMan::Log::GetCoreLogger()->info(__VA_ARGS__)
#define RM_CORE_WARN(...)     RoMan::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define RM_CORE_ERROR(...)    RoMan::Log::GetCoreLogger()->error(__VA_ARGS__)
#define RM_CORE_FATAL(...)    RoMan::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros

#define RM_TRACE(...)    RoMan::Log::GetClientLogger()->trace(__VA_ARGS__)
#define RM_INFO(...)     RoMan::Log::GetClientLogger()->info(__VA_ARGS__)
#define RM_WARN(...)     RoMan::Log::GetClientLogger()->warn(__VA_ARGS__)
#define RM_ERROR(...)    RoMan::Log::GetClientLogger()->error(__VA_ARGS__)
#define RM_FATAL(...)    RoMan::Log::GetClientLogger()->critical(__VA_ARGS__)
