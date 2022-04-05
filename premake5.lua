workspace "radrenderer"
	architecture "x64"
	startproject "radrenderer"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "radrenderer"
	location "radrenderer"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "pch.h"
	pchsource "PracticeEngine/src/pch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}
	
	includedirs
	{
		"%{prj.name}/src",
	}

	filter "configurations:Debug"
		defines
		{ 
			"PE_DEBUG", 
			"PE_ENABLE_ASSERTS"
		}
		buildoptions "/MTd"
		symbols "on"

	filter "configurations:Release"
		defines "PE_RELEASE"
		buildoptions "/MT"
		symbols "on"

	filter "configurations:Dist"
		defines "PE_DIST"
		buildoptions "/MT"
		symbols "on"
