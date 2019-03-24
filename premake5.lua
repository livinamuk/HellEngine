workspace "HellEngine"
	architecture "x64"
	startproject "Sandbox"

	configurations
	{
		"Debug",
		"Release",
		"Dist" 
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "HellEngine/vendor/GLFW/include"
IncludeDir["Glad"] = "HellEngine/vendor/Glad/include"
IncludeDir["ImGui"] = "HellEngine/vendor/imgui/"
 
 group "Dependencies"
	include "HellEngine/vendor/GLFW"
	include "HellEngine/vendor/Glad"
	include "HellEngine/vendor/imgui"

project "HellEngine"
	location "HellEngine"
	kind "SharedLib"
	language "C++"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "hellpch.h"
	pchsource "HellEngine/src/hellpch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}"
	}

	links 
	{ 
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"HELL_PLATFORM_WINDOWS",
			"HELL_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox/\"")
		}

	filter "configurations:Debug"
		defines "HELL_DEBUG" 
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "HELL_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines "HELL_DIST"
		runtime "Release"
		optimize "On"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"HellEngine/vendor/spdlog/include",
		"HellEngine/src"
	}

	links
	{
		"HellEngine"
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"HELL_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "HELL_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "HELL_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines "HELL_DIST"
		runtime "Release"
		optimize "On"

