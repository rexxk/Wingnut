#pragma once


#include "Platform/Vulkan/Buffer.h"
#include "Platform/Vulkan/Pipeline.h"
#include "Platform/Vulkan/RenderPass.h"
#include "Platform/Vulkan/Shader.h"


namespace Wingnut
{






	class SceneRenderer
	{
	public:
		static Ref<SceneRenderer> Create(VkExtent2D extent);

		SceneRenderer(VkExtent2D extent);
		~SceneRenderer();

		void Release();

		void BeginScene(uint32_t currentFrame);
		void EndScene();

		void Draw();

		void SubmitDescriptor(Ref<Vulkan::Descriptor> descriptor);
		void SubmitToDrawList(UUID entityID, const std::vector<Vertex>& vertexList, const std::vector<uint32_t>& indexList);

	private:
		void CreateRenderer();
		void UpdateEntityCache();

	private:

		VkExtent2D m_Extent;

		uint32_t m_CurrentFrame;
	};


}
