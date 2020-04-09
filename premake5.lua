workspace "RoMan"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "RoMan/vendor/GLFW/include"
IncludeDir["Glad"] = "RoMan/vendor/Glad/include"
IncludeDir["ImGui"] = "RoMan/vendor/imgui"
IncludeDir["glm"] = "RoMan/vendor/glm"
IncludeDir["stb_image"] = "RoMan/vendor/stb_image"


group "Dependencies"
	include "RoMan/vendor/GLFW"
	include "RoMan/vendor/Glad"
	include "RoMan/vendor/imgui"

group ""

project "RoMan"
	location "RoMan"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "rmpch.h"
	pchsource "RoMan/src/rmpch.cpp"

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/stb_image/**.h",
		"%{prj.name}/vendor/stb_image/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}"
	}

	links 
	{ 
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"RM_PLATFORM_WINDOWS",
			"RM_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

	filter "configurations:Debug"
		defines "RM_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "RM_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "RM_DIST"
		runtime "Release"
		optimize "on"


project "Colosseum"
	location "Colosseum"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"RoMan/vendor/spdlog/include;",
		"RoMan/src",
		"RoMan/vendor",
		"%{IncludeDir.glm}"
	}

	links
	{
		"RoMan"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"RM_PLATFORM_WINDOWS"
		}


	filter "configurations:Debug"
		defines "RM_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "RM_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "RM_DIST"
		runtime "Release"
		optimize "on"