#pragma once

#include "Platform/Vulkan/Buffer.h"
#include "Platform/Vulkan/Pipeline.h"


namespace Wingnut
{

	struct CameraDescriptorSet
	{
		glm::mat4 ViewProjection;
	};


	struct SceneData
	{
		Ref<Vulkan::Pipeline> GraphicsPipeline = nullptr;

		Ref<Vulkan::UniformBuffer> CameraData = nullptr;
	};


	struct SceneProperties
	{
		VkExtent2D PipelineExtent;
		Ref<Vulkan::RenderPass> MainRenderPass;
		Ref<Vulkan::Shader> GraphicsShader;
	};


	class Scene
	{
	public:
		Scene(const SceneProperties& properties);
		~Scene();

		void Release();

		void Begin();
		void End();

		SceneData& GetSceneData();

	private:

		SceneProperties m_Properties;
	};


}
