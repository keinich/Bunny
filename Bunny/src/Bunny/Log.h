#pragma once

#include "Core.h"
#include "spdlog/spdlog.h"

namespace Bunny {

  class BUNNY_API Log {
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
#define BN_CORE_FATAL(...)   ::Bunny::Log::GetCoreLogger()->critical(__VA_ARGS__)
#define BN_CORE_ERROR(...)   ::Bunny::Log::GetCoreLogger()->error(__VA_ARGS__)
#define BN_CORE_WARN(...)    ::Bunny::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define BN_CORE_INFO(...)    ::Bunny::Log::GetCoreLogger()->info(__VA_ARGS__)
#define BN_CORE_TRACE(...)   ::Bunny::Log::GetCoreLogger()->trace(__VA_ARGS__)

// Client log macros
#define BN_FATAL(...)        ::Bunny::Log::GetClientLogger()->critical(__VA_ARGS__)
#define BN_ERROR(...)        ::Bunny::Log::GetClientLogger()->error(__VA_ARGS__)
#define BN_WARN(...)         ::Bunny::Log::GetClientLogger()->warn(__VA_ARGS__)
#define BN_INFO(...)         ::Bunny::Log::GetClientLogger()->info(__VA_ARGS__)
#define BN_TRACE(...)        ::Bunny::Log::GetClientLogger()->trace(__VA_ARGS__)