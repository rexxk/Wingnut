project "vulkan"
    kind "StaticLib"
    language "C++"

	targetdir "%{wks.location}/bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}"
	objdir "%{wks.location}/bin-int/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}"



    filter "system:windows"
        systemversion "latest"
        cppdialect "c++20"
        staticruntime "on"

    filter "configurations:Debug"
		defines "NUT_DEBUG"
		symbols "on"
	
	filter "configurations:Release"
		optimize "on"
