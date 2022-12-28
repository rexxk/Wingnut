#include "MainLayer.h"

#include <glm/gtc/type_ptr.hpp>

using namespace Wingnut;


struct MVP
{
	glm::mat4 ViewProjection;
};


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


	m_CameraData = CreateRef<UniformBuffer>(rendererData.Device, sizeof(MVP));

}

void MainLayer::OnDetach()
{
	Renderer::GetContext()->GetRendererData().Device->WaitForIdle();

	m_CameraData->Release();

	m_TriangleVertexBuffer->Release();
	m_TriangleIndexBuffer->Release();
}

void MainLayer::OnUpdate()
{
	uint32_t currentFrame = Renderer::GetContext()->GetCurrentFrame();

	auto& rendererData = Renderer::GetContext()->GetRendererData();


	Renderer::BeginScene();

	MVP modelViewProjection;
	modelViewProjection.ViewProjection = glm::mat4(1.0f);

	m_CameraData->Update(&modelViewProjection, sizeof(modelViewProjection), Renderer::GetContext()->GetCurrentFrame());

	VkDescriptorBufferInfo bufferInfo = {};
	bufferInfo.buffer = m_CameraData->GetBuffer(Renderer::GetContext()->GetCurrentFrame());
	bufferInfo.offset = 0;
	bufferInfo.range = sizeof(MVP);

	VkWriteDescriptorSet setWrite = {};
	setWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	setWrite.dstBinding = 0;
	setWrite.dstSet = rendererData.Pipeline->GetSpecification().PipelineShader->GetDescriptorSets()[0];
	setWrite.descriptorCount = 1;
	setWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	setWrite.pBufferInfo = &bufferInfo;

	vkUpdateDescriptorSets(rendererData.Device->GetDevice(), 1, &setWrite, 0, nullptr);


	m_TriangleVertexBuffer->Bind(rendererData.GraphicsCommandBuffers[currentFrame], rendererData.Pipeline);
	m_TriangleIndexBuffer->Bind(rendererData.GraphicsCommandBuffers[currentFrame], rendererData.Pipeline);


	vkCmdBindDescriptorSets(rendererData.GraphicsCommandBuffers[currentFrame]->GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, rendererData.Pipeline->GetLayout(), 0, 1, &rendererData.Pipeline->GetSpecification().PipelineShader->GetDescriptorSets()[0],
		0, nullptr);


	vkCmdDrawIndexed(rendererData.GraphicsCommandBuffers[currentFrame]->GetCommandBuffer(), m_TriangleIndexBuffer->IndexCount(), 1, 0, 0, 0);


	Renderer::EndScene();
}
