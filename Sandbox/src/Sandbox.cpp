
#include "Wingnut.h"
#include "Core/Entry.h"



Wingnut::Application* CreateApplication()
{
	Wingnut::ApplicationProperties applicationProps;

	LOG_WARN("Application: {0}", applicationProps.Title);

	return new Wingnut::Application(applicationProps);
}
