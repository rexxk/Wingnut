
#include "Wingnut.h"
#include "Core/Entry.h"

#include "MainLayer.h"



class SandboxApplication : public Wingnut::Application
{
public:
	SandboxApplication(const Wingnut::ApplicationProperties& applicationProperties)
		: Application(applicationProperties)
	{
		AttachLayer(CreateRef<MainLayer>("mainLayer"));
	}

};




Scope<Wingnut::Application> CreateApplication()
{
	Wingnut::ApplicationProperties applicationProps;

	LOG_WARN("Application: {0}", applicationProps.Title);

	return CreateScope<SandboxApplication>(applicationProps);
}
