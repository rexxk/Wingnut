#include <iostream>

#include "Core/Entry.h"
#include "Wingnut.h"



Scope<Wingnut::Application> CreateApplication()
{
	Wingnut::ApplicationProperties applicationProps;

	LOG_WARN("Application: {0}", applicationProps.Title);

	return CreateScope<Wingnut::Application>(applicationProps);
}
