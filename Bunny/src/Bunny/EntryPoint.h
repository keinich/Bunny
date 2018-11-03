#pragma once

#ifdef BN_PLATFORM_WINDOWS

extern Bunny::Application* Bunny::CreateApplication();

int main(int argc, char** argv)
{
  auto app = Bunny::CreateApplication();
  app->Run();
  delete app;

  return 0;
}

#endif