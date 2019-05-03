workspace "ParticleSimulation"
	architecture "x64"
	startproject "ParticleSimulation"
	
	configurations { "Debug", "Release" }

include "vendor/GLEW"
include "vendor/GLFW"

project "ParticleSimulation"
	kind "ConsoleApp"
	language "C++"
	systemversion "latest"
	targetdir "bin/%{cfg.buildcfg}/ParticleSimulation"
	objdir "bin-int/%{cfg.buildcfg}/ParticleSimulation"

	files {
		"src/**.cpp",
		"src/**.h"
	}

	includedirs {
		"vendor/GLEW/include",
		"vendor/GLFW/include"
	}

	links {
		"GLEW",
		"GLFW"
	}

	filter "system:windows"
		links {
			"opengl32.lib"
		}

	filter "system:linux" 
		links {
			"X11",
			"dl",
			"GL",
			"pthread"
		}

	filter "configurations:Debug"
		symbols "On"
		runtime "Debug"

	filter "configurations:Release"
		optimize "On"
		runtime "Release"
