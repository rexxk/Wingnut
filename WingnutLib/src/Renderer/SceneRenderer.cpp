#include "wingnut_pch.h"
#include "SceneRenderer.h"


#include "Assets/ShaderStore.h"

#include "Event/EventUtils.h"
#include "Event/WindowEvents.h"

#include "Platform/Vulkan/Buffer.h"
#include "Platform/Vulkan/Image.h"
#include "Platform/Vulkan/RenderPass.h"

#include "Renderer/Renderer.h"


namespace Wingnut
{


	struct SceneItem
	{
		Ref<Vulkan::VertexBuffer> VertexBuffer;
		Ref<Vulkan::IndexBuffer> IndexBuffer;

		Ref<Vulkan::Descriptor> TransformDescriptor = nullptr;
		Ref<Vulkan::UniformBuffer> TransformBuffer = nullptr;

		Ref<Material> Material = nullptr;

		bool operator==(const SceneItem& other) const
		{
			return (VertexBuffer == other.VertexBuffer) && (IndexBuffer == other.IndexBuffer) && (TransformDescriptor == other.TransformDescriptor) && (TransformBuffer == other.TransformBuffer);
		}
	};


	struct SceneData
	{
		Ref<Vulkan::Shader> StaticSceneShader = nullptr;
		Ref<Vulkan::Shader> DynamicSceneShader = nullptr;
		Ref<Vulkan::Pipeline> StaticPipeline = nullptr;
		Ref<Vulkan::Pipeline> DynamicPipeline = nullptr;

//		std::unordered_map<UUID, std::pair<Ref<Vulkan::VertexBuffer>, Ref<Vulkan::IndexBuffer>>> DrawList;
//		std::unordered_map<UUID, std::pair<Ref<Vulkan::VertexBuffer>, Ref<Vulkan::IndexBuffer>>> DrawCache;
		std::unordered_map<UUID, SceneItem> DrawList;
		std::unordered_map<UUID, SceneItem> DrawCache;

		std::vector<Ref<Vulkan::Descriptor>> DescriptorList;
	};


	static SceneData s_SceneData;


	Ref<SceneRenderer> SceneRenderer::Create(VkExtent2D extent)
	{
		return CreateRef<SceneRenderer>(extent);
	}


	SceneRenderer::SceneRenderer(VkExtent2D extent)
		: m_Extent(extent)
	{
		CreateRenderer();


		SubscribeToEvent<WindowResizedEvent>([&](WindowResizedEvent& event)
			{
				if (event.Width() == 0 || event.Height() == 0)
					return false;

				m_Extent.width = event.Width();
				m_Extent.height = event.Height();

				Release();
				CreateRenderer();

				return false;
			});
	}

	SceneRenderer::~SceneRenderer()
	{
		void Release();
	}

	void SceneRenderer::Release()
	{

		s_SceneData.DrawList.clear();
		s_SceneData.DrawCache.clear();

		if (m_RenderImage != nullptr)
		{
			m_RenderImage->Release();
		}

		if (s_SceneData.StaticPipeline != nullptr)
		{
			s_SceneData.StaticPipeline->Release();
			s_SceneData.StaticPipeline = nullptr;
		}

		if (s_SceneData.DynamicPipeline != nullptr)
		{
			s_SceneData.DynamicPipeline->Release();
			s_SceneData.DynamicPipeline = nullptr;
		}

	}

	void SceneRenderer::CreateRenderer()
	{
		LOG_CORE_TRACE("[SceneRenderer] Creating renderer");

		auto& rendererData = Renderer::GetContext()->GetRendererData();
		uint32_t framesInflight = Renderer::GetRendererSettings().FramesInFlight;

		s_SceneData.StaticSceneShader = ShaderStore::GetShader(ShaderType::Default);

		Vulkan::PipelineSpecification pipelineSpecification;
		pipelineSpecification.Extent = m_Extent;
		pipelineSpecification.PipelineShader = s_SceneData.StaticSceneShader;
		pipelineSpecification.CullMode = Vulkan::CullMode::Back;
//		pipelineSpecification.CullMode = Vulkan::CullMode::None;
		pipelineSpecification.CullingDirection = Vulkan::CullingDirection::CounterClockwise;
//		pipelineSpecification.CullingDirection = Vulkan::CullingDirection::Clockwise;
		pipelineSpecification.RenderPass = rendererData.SceneRenderPass;
		pipelineSpecification.DepthTestEnable = true;
		pipelineSpecification.DepthWriteEnable = true;

//		pipelineSpecification.BlendEnable = false;

		s_SceneData.StaticPipeline = Vulkan::Pipeline::Create(rendererData.Device, pipelineSpecification);


		m_RenderImage = Vulkan::Image::Create(rendererData.Device, Vulkan::ImageType::Texture2D, m_Extent.width, m_Extent.height, VK_FORMAT_R8G8B8A8_UNORM,
			VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);

//		m_RenderImage->TransitionLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	}

	void SceneRenderer::BeginScene(uint32_t currentFrame)
	{
		auto& rendererData = Renderer::GetContext()->GetRendererData();
		auto& commandBuffer = rendererData.GraphicsCommandBuffers[currentFrame];

		s_SceneData.DescriptorList.clear();

		UpdateEntityCache();

		s_SceneData.DrawList.clear();

		m_CurrentFrame = currentFrame;

		vkCmdBindPipeline(commandBuffer->GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, s_SceneData.StaticPipeline->GetPipeline());

		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)m_Extent.width;
		viewport.height = (float)m_Extent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer->GetCommandBuffer(), 0, 1, &viewport);

		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = m_Extent;
		vkCmdSetScissor(commandBuffer->GetCommandBuffer(), 0, 1, &scissor);
	}

	void SceneRenderer::EndScene()
	{
		auto& rendererData = Renderer::GetContext()->GetRendererData();
		auto& commandBuffer = rendererData.GraphicsCommandBuffers[m_CurrentFrame];

		vkCmdEndRenderPass(commandBuffer->GetCommandBuffer());
	}

	void SceneRenderer::Draw()
	{
		auto& rendererData = Renderer::GetContext()->GetRendererData();
		auto& commandBuffer = rendererData.GraphicsCommandBuffers[m_CurrentFrame];

		vkCmdBindPipeline(commandBuffer->GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, s_SceneData.StaticPipeline->GetPipeline());

		for (auto& descriptor : s_SceneData.DescriptorList)
		{
			descriptor->Bind(commandBuffer, s_SceneData.StaticPipeline->GetLayout());
		}

		for (auto& entity : s_SceneData.DrawList)
		{
			SceneItem& sceneItem = entity.second;

			sceneItem.TransformDescriptor->Bind(commandBuffer, s_SceneData.StaticPipeline->GetLayout());
			sceneItem.Material->GetDescriptor()->Bind(commandBuffer, s_SceneData.StaticPipeline->GetLayout());

			VkBuffer vertexBuffers[] = { sceneItem.VertexBuffer->GetBuffer() };
			VkDeviceSize offsets[] = { 0 };

			vkCmdBindVertexBuffers(commandBuffer->GetCommandBuffer(), 0, 1, vertexBuffers, offsets);
			vkCmdBindIndexBuffer(commandBuffer->GetCommandBuffer(), sceneItem.IndexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);

			vkCmdDrawIndexed(commandBuffer->GetCommandBuffer(), sceneItem.IndexBuffer->IndexCount(), 1, 0, 0, 0);
		}

	}

	void SceneRenderer::SubmitDescriptor(Ref<Vulkan::Descriptor> descriptor)
	{
		s_SceneData.DescriptorList.emplace_back(descriptor);
	}

	void SceneRenderer::SubmitToDrawList(UUID entityID, const std::vector<Vertex>& vertexList, const std::vector<uint32_t>& indexList, const glm::mat4& transform, Ref<Material> material)
	{
		if (s_SceneData.DrawCache.find(entityID) == s_SceneData.DrawCache.end())
		{
			auto& device = Renderer::GetContext()->GetRendererData().Device;

			Ref<Vulkan::VertexBuffer> vertexBuffer = Vulkan::VertexBuffer::Create(device, vertexList.data(), (uint32_t)vertexList.size() * sizeof(Vertex));
			Ref<Vulkan::IndexBuffer> indexBuffer = Vulkan::IndexBuffer::Create(device, indexList.data(), (uint32_t)indexList.size() * sizeof(uint32_t), (uint32_t)indexList.size());

			Ref<Vulkan::UniformBuffer> transformBuffer = Vulkan::UniformBuffer::Create(device, sizeof(glm::mat4));
			Ref<Vulkan::Descriptor> transformDescriptor = Vulkan::Descriptor::Create(device, s_SceneData.StaticSceneShader, TransformDescriptor);
			transformDescriptor->SetBufferBinding(0, transformBuffer);
			transformDescriptor->UpdateBindings();

//			s_SceneData.DrawCache[entityID] = std::make_pair(vertexBuffer, indexBuffer);
			s_SceneData.DrawCache[entityID] = { vertexBuffer, indexBuffer, transformDescriptor, transformBuffer, material };

			Application::Get().GetMetrics().PolygonCount += indexBuffer->IndexCount() / 3;
		}
		else
		{
			s_SceneData.DrawCache[entityID].TransformBuffer->Update((void*)&transform, sizeof(glm::mat4), m_CurrentFrame);
			s_SceneData.DrawCache[entityID].Material = material;
		}

		s_SceneData.DrawList[entityID] = s_SceneData.DrawCache[entityID];
	}

	void SceneRenderer::UpdateEntityCache()
	{
		for (auto& entity : s_SceneData.DrawCache)
		{
			if (std::find(s_SceneData.DrawList.begin(), s_SceneData.DrawList.end(), entity) == s_SceneData.DrawList.end())
			{
				s_SceneData.DrawCache.erase(s_SceneData.DrawCache.find(entity.first));
			}
		}
	}

	Ref<Vulkan::Shader> SceneRenderer::GetShader()
	{
		return s_SceneData.StaticSceneShader;
	}

}
