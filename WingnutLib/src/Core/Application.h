#pragma once



namespace Wingnut
{

	struct ApplicationProperties
	{
		std::string Title = "Wingnut Engine";
	};



	class Application
	{
	public:
		Application(const ApplicationProperties& properties);


		void Run();

	};


}
