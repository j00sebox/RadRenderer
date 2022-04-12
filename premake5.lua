workspace "radrenderer"
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

	pchheader "pch.h"

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

	filter "system:windows"
		architecture "x86"
		systemversion "latest"

		pchsource "radrenderer/src/pch.cpp"

		defines { "PLATFORM_WINDOWS", "SFML_STATIC" }

		includedirs
		{
			"%{prj.name}/vendor/sfml/include"
		}

		libdirs { "%{prj.name}/vendor/sfml/lib" }

		links
		{
			"opengl32.lib", "freetype.lib", "winmm.lib", "gdi32.lib", "openal32.lib", "flac.lib", "vorbisenc.lib", "vorbisfile.lib", "vorbis.lib", "ogg.lib", "ws2_32.lib"
		}

	filter "system:linux"
		architecture "x64"
		systemversion "latest"

		defines { "PLATFORM_LINUX" }

		libdirs { "/usr/lib/x86_64-linux-gnu/" }

		links { "sfml-graphics", "sfml-window", "sfml-system" }

	filter { "system:windows", "configurations:Debug" }
		defines "DEBUG"
		runtime "Debug"
		symbols "On"

		links { "sfml-audio-s-d.lib", "sfml-graphics-s-d.lib", "sfml-network-s-d.lib", "sfml-system-s-d.lib", "sfml-window-s-d.lib" }

	filter { "system:windows", "configurations:Release" }
		defines "RELEASE"
		runtime "Release"
		optimize "On"

		links { "sfml-audio-s.lib", "sfml-graphics-s.lib", "sfml-network-s.lib", "sfml-system-s.lib", "sfml-window-s.lib" }
