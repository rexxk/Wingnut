#pragma once

#include "Device.h"

#include <vulkan/vulkan.h>


namespace Wingnut
{
	namespace Vulkan
	{

		enum class RenderTarget
		{
			Screen,
			Image,
		};

		enum class AttachmentLoadOp
		{
			Clear,
			DontCare,
			Load,
		};
		
		enum class AttachmentStoreOp
		{
			DontCare,
			None,
			Store,
		};

		struct RenderPassSpecification
		{
			AttachmentLoadOp LoadOp;
			AttachmentStoreOp StoreOp;

			RenderTarget Target;
			VkFormat Format;
		};


		class RenderPass
		{
		public:
			static Ref<RenderPass> Create(Ref<Device> device, const RenderPassSpecification& specification);

			RenderPass(Ref<Device> device, const RenderPassSpecification& specification);
			~RenderPass();

			void Release();

			VkRenderPass GetRenderPass() { return m_RenderPass; }

		private:
			void CreateRenderPass();

		private:
			VkRenderPass m_RenderPass = nullptr;

			VkDevice m_Device = nullptr;

			RenderPassSpecification m_Specification;
		};

	}
}
