project "GLEW"
	kind "StaticLib"
	language "C"
	targetdir "bin/%{cfg.buildcfg}"
	objdir "bin-int/%{cfg.buildcfg}"

	files {
		"src/**.c",
		"include/GL/**.h"
	}

	defines "GLEW_STATIC"

	filter "configurations:Debug"
		symbols "On"

	filter "configurations:Release"
		optimize "On"