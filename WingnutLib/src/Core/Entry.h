#pragma once

#include "Core/Application.h"
#include "Core/Log.h"

extern Wingnut::Application* CreateApplication();



int main()
{
	Wingnut::Log::Init();

	Wingnut::Application* application = nullptr;
	
	application = CreateApplication();

	if (application)
		application->Run();

	return 0;
}
