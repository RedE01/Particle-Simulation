project "GLEW"
	kind "StaticLib"
	language "C"
	systemversion "latest"
	targetdir "bin/%{cfg.buildcfg}"
	objdir "bin-int/%{cfg.buildcfg}"

	files {
		"src/**.c",
		"include/GL/**.h"
	}

	includedirs {
		"include"
	}

	filter "system:windows"
		links {
			"opengl32.lib"
		}

	defines "GLEW_STATIC"

	filter "configurations:Debug"
		symbols "On"

	filter "configurations:Release"
		optimize "On"