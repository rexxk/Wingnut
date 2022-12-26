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
		{ { -0.5f,  0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
		{ {  0.0f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
		{ {  0.5f,  0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
	};

	std::vector<uint32_t> triangleIndices =
	{
		0, 1, 2,
	};

	auto& rendererData = Renderer::GetContext()->GetRendererData();

	m_TriangleVertexBuffer = CreateRef<VertexBuffer>(rendererData.Device, triangleVertices);

	m_TriangleIndexBuffer = CreateRef<IndexBuffer>(rendererData.Device, triangleIndices);

//	Ref<Shader> testShader = CreateRef<Shader>(rendererData.Device, "assets/shaders/basic.shader");


}

void MainLayer::OnDetach()
{
	Renderer::GetContext()->GetRendererData().Device->WaitForIdle();

	m_TriangleVertexBuffer->Release();
	m_TriangleIndexBuffer->Release();
}

void MainLayer::OnUpdate()
{
	uint32_t currentFrame = Renderer::GetContext()->GetCurrentFrame();

	auto& rendererData = Renderer::GetContext()->GetRendererData();


	Renderer::BeginScene();


	m_TriangleVertexBuffer->Bind(rendererData.GraphicsCommandBuffers[currentFrame], rendererData.Pipeline);
	m_TriangleIndexBuffer->Bind(rendererData.GraphicsCommandBuffers[currentFrame], rendererData.Pipeline);

	vkCmdDrawIndexed(rendererData.GraphicsCommandBuffers[currentFrame]->GetCommandBuffer(), m_TriangleIndexBuffer->IndexCount(), 1, 0, 0, 0);


	Renderer::EndScene();
}
