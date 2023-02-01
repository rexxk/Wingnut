#include "wingnut_pch.h"
#include "ImGuiRenderer.h"


#include "Assets/ShaderStore.h"

#include "Event/EventUtils.h"
#include "Event/WindowEvents.h"

#include "Platform/Vulkan/Buffer.h"
#include "Platform/Vulkan/Image.h"

#include "Renderer/Renderer.h"



namespace Wingnut
{


	struct ImGuiSceneData
	{
		Ref<Vulkan::Shader> Shader = nullptr;
		Ref<Vulkan::Pipeline> Pipeline = nullptr;

		std::unordered_map<UUID, std::pair<Ref<Vulkan::VertexBuffer>, Ref<Vulkan::IndexBuffer>>> DrawList;
		std::unordered_map<UUID, std::pair<Ref<Vulkan::VertexBuffer>, Ref<Vulkan::IndexBuffer>>> DrawCache;

	};

	static ImGuiSceneData s_ImGuiSceneData;


	VkIndexType DataSizeToIndexType(uint32_t size)
	{
		switch (size)
		{
			case 1: return VK_INDEX_TYPE_UINT8_EXT;
			case 2: return VK_INDEX_TYPE_UINT16;
			case 4: return VK_INDEX_TYPE_UINT32;
		}

		return VK_INDEX_TYPE_UINT32;
	}


	Ref<ImGuiRenderer> ImGuiRenderer::Create(VkExtent2D extent)
	{
		return CreateRef<ImGuiRenderer>(extent);
	}


	ImGuiRenderer::ImGuiRenderer(VkExtent2D extent)
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

	ImGuiRenderer::~ImGuiRenderer()
	{
		void Release();
	}

	void ImGuiRenderer::Release()
	{

		s_ImGuiSceneData.DrawList.clear();
		s_ImGuiSceneData.DrawCache.clear();

		if (m_VertexBuffer != nullptr)
		{
			m_VertexBuffer->Release();
		}

		if (m_IndexBuffer != nullptr)
		{
			m_IndexBuffer->Release();
		}

		if (s_ImGuiSceneData.Pipeline != nullptr)
		{
			s_ImGuiSceneData.Pipeline->Release();
			s_ImGuiSceneData.Pipeline = nullptr;
		}

	}

	VkPipelineLayout ImGuiRenderer::GetPipelineLayout()
	{
		return s_ImGuiSceneData.Pipeline->GetLayout();
	}

	void ImGuiRenderer::CreateRenderer()
	{
		LOG_CORE_TRACE("[ImGuiRenderer] Creating renderer");

		auto& rendererData = Renderer::GetContext()->GetRendererData();
		uint32_t framesInflight = Renderer::GetRendererSettings().FramesInFlight;

		s_ImGuiSceneData.Shader = ShaderStore::GetShader("ImGui");

		Vulkan::PipelineSpecification pipelineSpecification;
		pipelineSpecification.Extent = m_Extent;
		pipelineSpecification.PipelineShader = s_ImGuiSceneData.Shader;
		pipelineSpecification.RenderPass = rendererData.RenderPass;
		pipelineSpecification.CullMode = Vulkan::CullMode::None;
		pipelineSpecification.CullingDirection = Vulkan::CullingDirection::CounterClockwise;
		pipelineSpecification.DepthTestEnable = false;
		pipelineSpecification.DepthWriteEnable = false;
		pipelineSpecification.DepthCompareOp = Vulkan::CompareOperation::LessOrEqual;

		s_ImGuiSceneData.Pipeline = Vulkan::Pipeline::Create(rendererData.Device, pipelineSpecification);
	}

	void ImGuiRenderer::BeginScene(uint32_t currentFrame)
	{
		auto& rendererData = Renderer::GetContext()->GetRendererData();
		auto& commandBuffer = rendererData.GraphicsCommandBuffers[currentFrame];

		UpdateEntityCache();

		s_ImGuiSceneData.DrawList.clear();

		m_CurrentFrame = currentFrame;

		vkCmdBindPipeline(commandBuffer->GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, s_ImGuiSceneData.Pipeline->GetPipeline());

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

	void ImGuiRenderer::EndScene()
	{

	}


	void ImGuiRenderer::Bind()
	{
		auto& rendererData = Renderer::GetContext()->GetRendererData();
		auto& commandBuffer = rendererData.GraphicsCommandBuffers[m_CurrentFrame];

		vkCmdBindPipeline(commandBuffer->GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, s_ImGuiSceneData.Pipeline->GetPipeline());

		VkBuffer vertexBuffers[] = { m_VertexBuffer->GetBuffer() };
		VkDeviceSize offsets[] = { 0 };

		vkCmdBindVertexBuffers(commandBuffer->GetCommandBuffer(), 0, 1, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(commandBuffer->GetCommandBuffer(), m_IndexBuffer->GetBuffer(), 0, DataSizeToIndexType(sizeof(ImDrawIdx)));

	}

	void ImGuiRenderer::SubmitBuffers(const std::vector<ImDrawVert>& vertexList, const std::vector<ImDrawIdx>& indexList)
	{

		if (m_VertexBuffer == nullptr)
		{
			m_VertexBuffer = Vulkan::VertexBuffer::Create(Renderer::GetContext()->GetRendererData().Device, vertexList.data(), (uint32_t)vertexList.size() * sizeof(ImDrawVert));
		}
		else
		{
			m_VertexBuffer->SetData(vertexList.data(), (uint32_t)vertexList.size() * sizeof(ImDrawVert));
		}

		if (m_IndexBuffer == nullptr)
		{
			m_IndexBuffer = Vulkan::IndexBuffer::Create(Renderer::GetContext()->GetRendererData().Device, indexList.data(), (uint32_t)indexList.size() * sizeof(ImDrawIdx), (uint32_t)indexList.size());
		}
		else
		{
			m_IndexBuffer->SetData(indexList.data(), (uint32_t)indexList.size() * sizeof(ImDrawIdx), (uint32_t)indexList.size());
		}

	}

	void ImGuiRenderer::UpdateEntityCache()
	{
		for (auto& entity : s_ImGuiSceneData.DrawCache)
		{
			if (std::find(s_ImGuiSceneData.DrawList.begin(), s_ImGuiSceneData.DrawList.end(), entity) == s_ImGuiSceneData.DrawList.end())
			{
				s_ImGuiSceneData.DrawCache.erase(s_ImGuiSceneData.DrawCache.find(entity.first));
			}
		}
	}



}
