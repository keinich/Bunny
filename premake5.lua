workspace "Bunny"
  architecture "x64"
  
  configurations 
  {
    "Debug",
    "Release",
    "Dist"
  }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Bunny"
  location "Bunny"
  kind "SharedLib"
  language "C++"

  targetdir ("bin/" .. outputdir  .. "/%{prj.name}")
  objdir ("bin-int/" .. outputdir  .. "/%{prj.name}")

  files 
  {
    "%{prj.name}/src/**.h",
    "%{prj.name}/src/**.cpp"
  }

  includedirs
  {
    "%{prj.name}/vendor/spdlog/include"
  }

  links
  {
    "d3d12",
    "dxgi",
    "d3dcompiler"
  }

  filter "system:windows"
    cppdialect "C++17"
    staticruntime "On"
    systemversion "10.0.17763.0"

    defines 
    {    
      "BN_PLATFORM_WINDOWS",
      "BN_BUILD_DLL",
      "_WINDLL"
    }

    postbuildcommands
    {
      ("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
    }

  filter "configurations:Debug"
    defines "BN_DEBUG"
    symbols "On"

  filter "configurations:Release"
    defines "BN_RELEASE"
    optimize "On"

  filter "configurations:Dist"
    defines "BN_DIST"
    optimize "On"

project "Sandbox"
  location "Sandbox"
  kind "ConsoleApp"
  language "C++"

  targetdir ("bin/" .. outputdir  .. "/%{prj.name}")
  objdir ("bin-int/" .. outputdir  .. "/%{prj.name}")

  files 
  {
    "%{prj.name}/src/**.h",
    "%{prj.name}/src/**.cpp"
  }

  includedirs 
  {
    "Bunny/vendor/spdlog/include",
    "Bunny/src"
  }

  links
  {
    "Bunny"
  }

  filter "system:windows"
    cppdialect "C++17"
    staticruntime "On"
    systemversion "10.0.17763.0"

    defines 
    {    
      "BN_PLATFORM_WINDOWS",
      "_WINDLL"
    }

  filter "configurations:Debug"
    defines "BN_DEBUG"
    symbols "On"

  filter "configurations:Release"
    defines "BN_RELEASE"
    optimize "On"

  filter "configurations:Dist"
    defines "BN_DIST"
    optimize "On"