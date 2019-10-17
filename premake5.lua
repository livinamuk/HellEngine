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
IncludeDir["glm"] = "HellEngine/vendor/glm/include"
IncludeDir["ImGui"] = "HellEngine/vendor/imgui/"
IncludeDir["glm"] = "HellEngine/vendor/glm"
IncludeDir["stb"] = "HellEngine/vendor/stb_image"
IncludeDir["assimp"] = "HellEngine/vendor/assimp/include"
IncludeDir["assimpcfg"] = "HellEngine/vendor/assimp/config"
IncludeDir["rapidjson"] = "HellEngine/vendor/rapidjson/include"
IncludeDir["fmod"] = "HellEngine/vendor/fmod/include"
IncludeDir["react"] = "HellEngine/vendor/react/include"
IncludeDir["react"] = "HellEngine/vendor/bullet/"

group "Dependencies"
	include "HellEngine/vendor/GLFW"
	include "HellEngine/vendor/Glad"
	include "HellEngine/vendor/imgui"
	include "HellEngine/vendor/assimp"

project "HellEngine"
	location "HellEngine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "hellpch.h"
	pchsource "HellEngine/src/hellpch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl",
		"%{prj.name}/vendor/fmod/include/**.h",
		"%{prj.name}/vendor/fmod/include/**.hpp",
		"%{prj.name}/vendor/fmod/include/**.cpp"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{prj.name}/vendor/stb_image",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.assimp}",
		"%{IncludeDir.assimpcfg}",
		"%{IncludeDir.rapidjson}",
		"%{IncludeDir.fmod}",
		"%{IncludeDir.react}",
		"%{IncludeDir.bullet}"
	}

	libdirs 
	{ 
		--"HellEngine/vendor/react/lib",
		--"HellEngine/vendor/bullet/lib/debug/"
	}
	
	links 
	{ 
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib",
		"assimp",
		--"reactphysics3d.lib",
		--"Bullet3Dynamics_Debug.lib",
		--"BulletCollision_Debug.lib",
		--"LinearMath_Debug.lib",
		--"reactphysics3d.lib"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"HELL_PLATFORM_WINDOWS",
			"HELL_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

	filter "configurations:Debug"
		defines "HELL_DEBUG" 
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "HELL_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "HELL_DIST"
		runtime "Release"
		optimize "on"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/wave/**.hpp",
		"%{prj.name}/vendor/wave/**.cpp"
	}

	includedirs
	{
		"HellEngine/vendor/spdlog/include",
		"HellEngine/src",
		"HellEngine/vendor",
		"%{IncludeDir.glm}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.assimp}",
		"%{IncludeDir.assimpcfg}",
		"%{IncludeDir.rapidjson}",
		"%{IncludeDir.react}",
		"%{IncludeDir.bullet}"
	}
	
	libdirs 
	{ 
		"HellEngine/vendor/fmod/lib/x64",
		"HellEngine/vendor/react/lib",
		"HellEngine/vendor/bullet/bullet/lib/release/"
	}

	links
	{	
		"GLFW",
		"Glad",
		"HellEngine",
		"assimp",
		"fmodL_vc.lib",
		"reactphysics3d.lib",

		"Bullet3Dynamics.lib",
		"BulletDynamics.lib",
		"BulletCollision.lib",
		"LinearMath.lib",
		--"Bullet2FileLoader.lib",
		--"Bullet3Collision.lib",
		--"Bullet3Common.lib",
		--"Bullet3Dynamics.lib",
		--"Bullet3Geometry.lib",
		--"Bullet3OpenCL_clew.lib",
		--"BulletCollision.lib",
		--"BulletDynamics.lib",
	--	"BulletExampleBrowserLib.lib",
	--	"BulletFileLoader.lib",
	--	"BulletInverseDynamics.lib",
	--	"BulletInverseDynamicsUtils.lib",
	--	"BulletRobotics.lib",
	--	"BulletSoftBody.lib",
		--"BulletWorldImporter.lib",
		--"BulletXmlWorldImporter.lib",
		--"BussIK.lib"


	}	

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"HELL_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "HELL_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "HELL_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "HELL_DIST"
		runtime "Release"
		optimize "on"

