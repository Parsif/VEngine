workspace "VEngine"
    configurations { "Debug", "Release" }

architecture "x86_64"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "VEngine/vendor/glfw"
include "VEngine/vendor/Vmath"

project "VEngine"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    targetdir ("bin-int/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/bin-int/" .. outputdir .. "/%{prj.name}")
    
    
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
        "%{prj.name}/vendor/Vmath/Vmath"
    }

    links
    {
        "opengl32.lib",
        "GLFW",
        "%{prj.name}/vendor/glew/lib/Release/x64/glew32s.lib",
        "Vmath"
    }

    filter "system:windows"
        staticruntime "On"
        systemversion "latest"


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
