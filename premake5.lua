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
		
		buildoptions "/MTd"
		symbols "on"

	filter "configurations:Release"
		buildoptions "/MT"
		symbols "on"

	filter "configurations:Dist"
		buildoptions "/MT"
		symbols "on"
