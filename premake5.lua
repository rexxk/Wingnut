-- premake5.lua
-- Main workspace

workspace "Wingnut"
	configurations { "Debug", "Release" }
	platforms { "Win64" }
	startproject "Sandbox"

	IncludeDir = {}
	IncludeDir["glm"] = "vendor/glm"
	IncludeDir["spdlog"] = "vendor/spdlog/include"
	IncludeDir["imgui"] = "vendor/imgui"
	IncludeDir["stbimage"] = "vendor/stb-image"

	targetdir "%{wks.location}/bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}"
	objdir "%{wks.location}/bin-int/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}"

	group "Dependencies"
		include "WingnutLib/vendor/imgui"
	group ""


group "Core"

project "WingnutLib"
	location "WingnutLib"
	kind "StaticLib"
	language "C++"
	cppdialect "c++20"

	
	files
	{
		"%{prj.location}/src/**.h",
		"%{prj.location}/src/**.cpp"
	}
	
	includedirs
	{
		"%{prj.location}/src",
		"%{prj.location}/%{IncludeDir.spdlog}",
		"%{prj.location}/%{IncludeDir.glm}",
		"%{prj.location}/%{IncludeDir.imgui}",
		"%{prj.location}/%{IncludeDir.stbimage}",
	}

	links
	{
		"ImGui",
	}

	pchheader "wingnut_pch.h"
	pchsource "%{prj.location}/src/wingnut_pch.cpp"

	defines 
	{
		"_CRT_SECURE_NO_WARNINGS",
	}
	
	filter "configurations:Debug"
		defines "NUT_DEBUG"
		symbols "on"

		links
		{
			"WingnutLib/vendor/assimp/lib/assimp-vc142-mtd.lib"
		}
	
	filter "configurations:Release"
		optimize "on"

		links
		{
			"WingnutLib/vendor/assimp/lib/assimp-vc142-mt.lib"
		}
	


group "Tools"

	
project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "c++20"

	targetdir "%{wks.location}/bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}"
	objdir "%{wks.location}/bin-int/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}"
	
	files 
	{
		"%{prj.location}/src/**.h",
		"%{prj.location}/src/**.cpp"
	}

	includedirs 
	{
		"%{wks.location}/WingnutLib/src",
		"%{wks.location}/WingnutLib/%{IncludeDir.spdlog}",
		"%{wks.location}/WingnutLib/%{IncludeDir.Glad}",
		"%{wks.location}/WingnutLib/%{IncludeDir.glm}",
		"%{wks.location}/WingnutLib/%{IncludeDir.imgui}",
	}
	
	links 
	{
		"WingnutLib"
	}
	
	filter "configurations:Debug"
		defines "NUT_DEBUG"
		symbols "on"

		postbuildcommands
		{
--			'{COPY} "../NutLib/vendor/assimp/bin/assimp-vc142-mtd.dll" "%{cfg.targetdir}"'
		}
	
	filter "configurations:Release"
		optimize "on"

		postbuildcommands
		{
--			'{COPY} "../NutLib/vendor/assimp/bin/assimp-vc142-mt.dll" "%{cfg.targetdir}"'
		}
