#pragma once

#include "Core/Application.h"
#include "Core/Base.h"
#include "Core/Log.h"

extern Scope<Wingnut::Application> CreateApplication();



int main()
{
	Wingnut::Log::Init();

	Scope<Wingnut::Application> application = nullptr;
	
	application = CreateApplication();

	if (application)
		application->Run();

	return 0;
}
