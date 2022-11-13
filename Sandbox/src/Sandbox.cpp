#include <iostream>

#include "Core/Entry.h"
#include "Wingnut.h"



Wingnut::Application* CreateApplication()
{
	Wingnut::ApplicationProperties applicationProps;

	LOG_WARN("Application: {0}", applicationProps.Title);

	return new Wingnut::Application(applicationProps);
}
