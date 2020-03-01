workspace "RoMan"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "RoMan"
	location "RoMan"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"RM_PLATFORM_WINDOWS",
			"RM_BUILD_DLL"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Colosseum")
		}


	filter "configurations:Debug"
		defines "RM_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "RM_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "RM_DIST"
		optimize "On"


project "Colosseum"
	location "Colosseum"
	kind "ConsoleApp"
	language "C++"

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
		"RoMan/src"
	}

	links
	{
		"RoMan"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"RM_PLATFORM_WINDOWS"
		}


	filter "configurations:Debug"
		defines "RM_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "RM_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "RM_DIST"
		optimize "On"