#include "wingnut_pch.h"
#include "Scene.h"

#include "Renderer/Renderer.h"


namespace Wingnut
{

	static SceneData s_SceneData;




	Scene::Scene(const SceneProperties& properties)
		: m_Properties(properties)
	{
		auto& rendererData = Renderer::GetContext()->GetRendererData();

		s_SceneData.CameraData = CreateRef<Vulkan::UniformBuffer>(rendererData.Device, sizeof(CameraDescriptorSet));

		Vulkan::PipelineSpecification pipelineSpecification;
		pipelineSpecification.Extent = properties.PipelineExtent;
		pipelineSpecification.PipelineShader = properties.GraphicsShader;
		pipelineSpecification.RenderPass = properties.MainRenderPass;

		s_SceneData.GraphicsPipeline = CreateRef<Vulkan::Pipeline>(rendererData.Device, pipelineSpecification);


	}

	Scene::~Scene()
	{

	}

	void Scene::Release()
	{
		if (s_SceneData.CameraData)
		{
			s_SceneData.CameraData->Release();
		}

		if (s_SceneData.GraphicsPipeline)
		{
			s_SceneData.GraphicsPipeline->Release();
		}
	}

	void Scene::Begin()
	{
		auto& rendererData = Renderer::GetContext()->GetRendererData();

		Renderer::BeginScene(s_SceneData.GraphicsPipeline);

		CameraDescriptorSet cameraDescriptorSet;
		cameraDescriptorSet.ViewProjection = glm::mat4(1.0f);

		s_SceneData.CameraData->Update(&cameraDescriptorSet, sizeof(CameraDescriptorSet), Renderer::GetContext()->GetCurrentFrame());

		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = s_SceneData.CameraData->GetBuffer(Renderer::GetContext()->GetCurrentFrame());
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(CameraDescriptorSet);

		VkWriteDescriptorSet setWrite = {};
		setWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		setWrite.dstBinding = 0;
		setWrite.dstSet = s_SceneData.GraphicsPipeline->GetSpecification().PipelineShader->GetDescriptorSets()[0];
		setWrite.descriptorCount = 1;
		setWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		setWrite.pBufferInfo = &bufferInfo;

		vkUpdateDescriptorSets(rendererData.Device->GetDevice(), 1, &setWrite, 0, nullptr);

	}

	void Scene::End()
	{
		Renderer::EndScene();
	}

	SceneData& Scene::GetSceneData()
	{
		return s_SceneData;
	}


}
