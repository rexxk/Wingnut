#include "MainLayer.h"

#include <glm/gtc/type_ptr.hpp>

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

	ShaderStore::LoadShader("basic", "assets/shaders/Basic.shader");

	auto& rendererData = Renderer::GetContext()->GetRendererData();

	auto extent = rendererData.Device->GetDeviceProperties().SurfaceCapabilities.currentExtent;

	m_Camera = Camera::Create(glm::vec3(0.0f, 0.0f, -10.0f), extent.width, extent.height);


	SceneProperties sceneProperties;
	sceneProperties.MainRenderPass = rendererData.RenderPass;
	sceneProperties.GraphicsShader = ShaderStore::GetShader("basic");
	sceneProperties.PipelineExtent = rendererData.Device->GetDeviceProperties().SurfaceCapabilities.currentExtent;

	sceneProperties.SceneCamera = m_Camera;

	m_Scene = CreateRef<Scene>(sceneProperties);


	m_TriangleVertexBuffer = CreateRef<VertexBuffer>(rendererData.Device, triangleVertices);
	m_TriangleIndexBuffer = CreateRef<IndexBuffer>(rendererData.Device, triangleIndices);

}

void MainLayer::OnDetach()
{
	Renderer::GetContext()->GetRendererData().Device->WaitForIdle();

	m_TriangleVertexBuffer->Release();
	m_TriangleIndexBuffer->Release();

	m_Scene->Release();
}

void MainLayer::OnUpdate()
{
	m_Scene->Begin();

	uint32_t currentFrame = Renderer::GetContext()->GetCurrentFrame();

	auto& rendererData = Renderer::GetContext()->GetRendererData();


	m_TriangleVertexBuffer->Bind(rendererData.GraphicsCommandBuffers[currentFrame], m_Scene->GetSceneData().GraphicsPipeline);
	m_TriangleIndexBuffer->Bind(rendererData.GraphicsCommandBuffers[currentFrame], m_Scene->GetSceneData().GraphicsPipeline);


	vkCmdBindDescriptorSets(rendererData.GraphicsCommandBuffers[currentFrame]->GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_Scene->GetSceneData().GraphicsPipeline->GetLayout(), 0, 1,
		&m_Scene->GetSceneData().GraphicsPipeline->GetSpecification().PipelineShader->GetDescriptorSets()[0], 0, nullptr);


	vkCmdDrawIndexed(rendererData.GraphicsCommandBuffers[currentFrame]->GetCommandBuffer(), m_TriangleIndexBuffer->IndexCount(), 1, 0, 0, 0);


	m_Scene->End();

}
