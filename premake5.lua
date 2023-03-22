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
	IncludeDir["vulkan"] = "vendor/vulkan/1.3.231.1/Include"
	IncludeDir["assimp"] = "vendor/assimp/include"

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
		"%{prj.location}/%{IncludeDir.vulkan}",
		"%{prj.location}/%{IncludeDir.assimp}",
	}

	links
	{
		"ImGui",
		"WingnutLib/vendor/vulkan/1.3.231.1/Lib/vulkan-1.lib",
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
			"WingnutLib/vendor/Vulkan/1.3.231.1/Lib/shaderc_combinedd.lib",
			"WingnutLib/vendor/Vulkan/1.3.231.1/Lib/MachineIndependentd.lib",
			"WingnutLib/vendor/Vulkan/1.3.231.1/Lib/OSDependentd.lib",
			"WingnutLib/vendor/Vulkan/1.3.231.1/Lib/SPIRVd.lib",
			"WingnutLib/vendor/Vulkan/1.3.231.1/Lib/SPIRV-Toolsd.lib",
			"WingnutLib/vendor/Vulkan/1.3.231.1/Lib/SPIRV-Tools-optd.lib",
			"WingnutLib/vendor/Vulkan/1.3.231.1/Lib/SPIRV-Tools-sharedd.lib",
			"WingnutLib/vendor/assimp/lib/assimp-vc143-mtd.lib",
			"WingnutLib/vendor/assimp/lib/zlibstaticd.lib",
		}
	
	filter "configurations:Release"
		optimize "on"

		links
		{
			"WingnutLib/vendor/Vulkan/1.3.231.1/Lib/shaderc_combined.lib",
			"WingnutLib/vendor/Vulkan/1.3.231.1/Lib/MachineIndependent.lib",
			"WingnutLib/vendor/Vulkan/1.3.231.1/Lib/OSDependent.lib",
			"WingnutLib/vendor/Vulkan/1.3.231.1/Lib/SPIRV.lib",
			"WingnutLib/vendor/Vulkan/1.3.231.1/Lib/SPIRV-Tools.lib",
			"WingnutLib/vendor/Vulkan/1.3.231.1/Lib/SPIRV-Tools-opt.lib",
			"WingnutLib/vendor/Vulkan/1.3.231.1/Lib/SPIRV-Tools-shared.lib",
			"WingnutLib/vendor/assimp/lib/assimp-vc143-mt.lib",
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
		"%{wks.location}/WingnutLib/%{IncludeDir.vulkan}",
	}
	
	links 
	{
		"WingnutLib"
	}
	
	filter "configurations:Debug"
		defines "NUT_DEBUG"
		symbols "on"

		links
		{
			"%{wks.location}/WingnutLib/vendor/assimp/lib/assimp-vc143-mtd.lib",
		}

		postbuildcommands
		{
			'{COPY} "../WingnutLib/vendor/assimp/bin/assimp-vc143-mtd.dll" "%{cfg.targetdir}"'
		}

	filter "configurations:Release"
		optimize "on"

		links
		{
			"%{wks.location}/WingnutLib/vendor/assimp/lib/assimp-vc143-mt.lib",
		}
		
		postbuildcommands
		{
			'{COPY} "../WingnutLib/vendor/assimp/bin/assimp-vc143-mt.dll" "%{cfg.targetdir}"'
		}
