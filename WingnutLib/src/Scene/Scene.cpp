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
		uint32_t currentFrame = Renderer::GetContext()->GetCurrentFrame();

		Renderer::BeginScene(s_SceneData.GraphicsPipeline);

		m_Properties.SceneCamera->Update();

		CameraDescriptorSet cameraDescriptorSet;
		cameraDescriptorSet.ViewProjection = m_Properties.SceneCamera->GetViewProjectionMatrix();

		s_SceneData.CameraData->Update(&cameraDescriptorSet, sizeof(CameraDescriptorSet), currentFrame);
		s_SceneData.GraphicsPipeline->UpdateDescriptor(0, 0, s_SceneData.CameraData->GetBuffer(currentFrame), sizeof(CameraDescriptorSet));

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
