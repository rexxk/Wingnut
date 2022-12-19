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

	std::vector<Vertex> triangleVertices =
	{
		{ { -0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
		{ {  0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
		{ {  0.0f,  0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
	};

	std::vector<uint32_t> triangleIndices =
	{
		0, 1, 2,
	};

	auto& rendererData = Renderer::GetRendererData();

	m_TriangleVertexBuffer = CreateRef<VertexBuffer>(rendererData.Device, triangleVertices);

	m_TriangleIndexBuffer = CreateRef<IndexBuffer>(rendererData.Device, triangleIndices);


}

void MainLayer::OnDetach()
{
	m_TriangleVertexBuffer->Release();
	m_TriangleIndexBuffer->Release();
}

void MainLayer::OnUpdate()
{
	Renderer::BeginScene();




	Renderer::EndScene();
}
