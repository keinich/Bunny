#pragma once

#ifdef BN_PLATFORM_WINDOWS

extern Bunny::Application* Bunny::CreateApplication();

int main(int argc, char** argv)
{
  Bunny::Log::Init();
  BN_CORE_WARN("Initialized Log!");
  int a = 5;
  BN_CORE_INFO("Hello! Var={0}", a);

  auto app = Bunny::CreateApplication();
  app->Run();
  delete app;

  return 0;
}

#endif