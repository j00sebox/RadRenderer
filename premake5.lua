workspace "radrenderer"
	architecture "x86"
	startproject "radrenderer"

	configurations
	{
		"Debug",
		"Release"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "radrenderer"
	location "radrenderer"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"

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
		"%{prj.name}/vendor/sfml/include"
	}

	libdirs { "%{prj.name}/vendor/sfml/lib" }

	links 
	{ 
		"opengl32.lib", "freetype.lib", "winmm.lib", "gdi32.lib", "openal32.lib", "flac.lib", "vorbisenc.lib", "vorbisfile.lib", "vorbis.lib", "ogg.lib", "ws2_32.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines { "PLATFORM_WINDOWS", "SFML_STATIC" }
		filter "configurations:Debug"
		

	filter "configurations:Debug"
		defines "DEBUG"
		runtime "Debug"
		symbols "On"

		links { "sfml-audio-s-d.lib", "sfml-graphics-s-d.lib", "sfml-network-s-d.lib", "sfml-system-s-d.lib", "sfml-window-s-d.lib" }

	  filter "configurations:Release"
		defines "RELEASE"
		runtime "Release"
		optimize "On"

		links { "sfml-audio-s.lib", "sfml-graphics-s.lib", "sfml-network-s.lib", "sfml-system-s.lib", "sfml-window-s.lib" }
