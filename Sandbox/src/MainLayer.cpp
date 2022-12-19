#include "MainLayer.h"

using namespace Wingnut;


MainLayer::MainLayer(const std::string& name)
	: Layer(name)
{

}

MainLayer::~MainLayer()
{

}


void MainLayer::OnAttach()
{
	LOG_TRACE("Attaching Main layer");
}

void MainLayer::OnUpdate()
{

	Renderer::BeginScene();


	Renderer::EndScene();
}
