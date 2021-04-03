workspace "VEngine"
    configurations { "Debug", "Release" }   
    startproject "VEngine"


architecture "x86_64"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"


include "VEngine/vendor/glfw/premake5.lua"
include "VEngine/vendor/imgui/premake5.lua"
include "VEngine/vendor/yaml/premake5.lua"


project "VEngine"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    targetdir ("bin-int/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/bin-int/" .. outputdir .. "/%{prj.name}")
    
    
    pchheader "precheader.h"
	pchsource "%{prj.name}/src/precheader.cpp"

    files 
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }


    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/vendor/glew/include",
        "%{prj.name}/vendor/glfw/include",
        "%{prj.name}/vendor/imgui",
        "%{prj.name}/vendor/glm",
        "%{prj.name}/vendor/assimp/include",
        "%{prj.name}/vendor/entt",
        "%{prj.name}/vendor/stb_image",
        "%{prj.name}/vendor/yaml/include",
    }

    links
    {
        "GLFW",
        "ImGui",
        "Yaml",
        "opengl32.lib",
        "%{prj.name}/vendor/glew/lib/Release/x64/glew32s.lib",
        "%{prj.name}/vendor/assimp/assimp.lib",
    }

    postbuildcommands
    {
        "{COPY} %{prj.name}/vendor/assimp/assimp-vc142-mt.dll %{cfg.targetdir}",
        "{COPY} %{prj.name}/vendor/assimp/draco.dll %{cfg.targetdir}",
    }

    filter "system:windows"
        staticruntime "On"
        systemversion "latest"

        warnings  "Extra" 

        defines
        {
            "V_PLATFORM_WINDOWS", "GLFW_INCLUDE_NONE", "GLEW_STATIC"
        }

    filter "configurations:Debug"
        symbols "On"
        defines { "V_CONFIGURATION_DEBUG" }

    filter "configurations:Release"
        optimize "On"
        defines { "V_CONFIGURATION_RELEASE" }