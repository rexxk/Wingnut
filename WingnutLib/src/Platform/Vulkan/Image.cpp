#include "wingnut_pch.h"
#include "Image.h"

#include "Buffer.h"

#include "Renderer/Renderer.h"


namespace Wingnut
{

	namespace Vulkan
	{

		Image::Image(Ref<Device> device, ImageType type, uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usageFlags, VkMemoryPropertyFlags memoryFlags, VkImageTiling tiling, VkImageAspectFlags aspectFlags)
			: m_Device(device), m_Format(format), m_Type(type)
		{
			CreateImage(width, height, usageFlags, memoryFlags, tiling);
			CreateImageView(aspectFlags);
		}


		Image::~Image()
		{
			Release();
		}

		void Image::Release()
		{
			if (m_ImageView != nullptr)
			{
				vkDestroyImageView(m_Device->GetDevice(), m_ImageView, nullptr);
				m_ImageView = nullptr;
			}

			if (m_ImageMemory != nullptr)
			{
				vkFreeMemory(m_Device->GetDevice(), m_ImageMemory, nullptr);
				m_ImageMemory = nullptr;
			}

			if (m_Image != nullptr)
			{
				vkDestroyImage(m_Device->GetDevice(), m_Image, nullptr);
				m_Image = nullptr;
			}
		}

		void Image::CreateImage(uint32_t width, uint32_t height, VkImageUsageFlags usageFlags, VkMemoryPropertyFlags memoryFlags, VkImageTiling tiling)
		{
			VkImageCreateInfo imageCreateInfo = {};
			imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
			imageCreateInfo.extent.width = (uint32_t)width;
			imageCreateInfo.extent.height = (uint32_t)height;
			imageCreateInfo.extent.depth = 1;

			imageCreateInfo.mipLevels = 1;
			imageCreateInfo.arrayLayers = 1;

			imageCreateInfo.format = m_Format;
			imageCreateInfo.tiling = tiling;
			imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

			imageCreateInfo.usage = usageFlags;
			imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			imageCreateInfo.flags = 0;

			if (vkCreateImage(m_Device->GetDevice(), &imageCreateInfo, nullptr, &m_Image) != VK_SUCCESS)
			{
				LOG_CORE_ERROR("[Texture] Unable to create texture image");
				return;
			}

			VkMemoryRequirements memoryRequirements;
			vkGetImageMemoryRequirements(m_Device->GetDevice(), m_Image, &memoryRequirements);

			VkMemoryAllocateInfo allocateInfo = {};
			allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocateInfo.allocationSize = memoryRequirements.size;
			allocateInfo.memoryTypeIndex = m_Device->FindMemoryType(memoryRequirements.memoryTypeBits, memoryFlags);

			if (vkAllocateMemory(m_Device->GetDevice(), &allocateInfo, nullptr, &m_ImageMemory) != VK_SUCCESS)
			{
				LOG_CORE_ERROR("[Texture] Unable to allocate texture image memory");
				return;
			}

			vkBindImageMemory(m_Device->GetDevice(), m_Image, m_ImageMemory, 0);
		}

		void Image::CreateImageView(VkImageAspectFlags aspectFlags)
		{
			VkImageViewCreateInfo imageViewCreateInfo = {};
			imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			imageViewCreateInfo.image = m_Image;
			imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			imageViewCreateInfo.format = m_Format;

			imageViewCreateInfo.subresourceRange.aspectMask = aspectFlags;
			imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
			imageViewCreateInfo.subresourceRange.levelCount = 1;
			imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
			imageViewCreateInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(m_Device->GetDevice(), &imageViewCreateInfo, nullptr, &m_ImageView) != VK_SUCCESS)
			{
				LOG_CORE_ERROR("[Texture] Unable to create texture image view");
				return;
			}
		}

		void Image::TransitionLayout(VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
		{
			auto commandPool = Renderer::GetContext()->GetRendererData().TransferCommandPool;

			Ref<CommandBuffer> commandBuffer = CreateRef<CommandBuffer>(m_Device, commandPool);

			commandBuffer->BeginRecording();

			VkPipelineStageFlags sourceStage;
			VkPipelineStageFlags destinationStage;

			VkImageMemoryBarrier memoryBarrier = {};
			memoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			memoryBarrier.oldLayout = oldLayout;
			memoryBarrier.newLayout = newLayout;

			memoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			memoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

			memoryBarrier.image = m_Image;
			memoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			memoryBarrier.subresourceRange.baseMipLevel = 0;
			memoryBarrier.subresourceRange.levelCount = 1;
			memoryBarrier.subresourceRange.baseArrayLayer = 0;
			memoryBarrier.subresourceRange.layerCount = 1;

			if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
			{
				memoryBarrier.srcAccessMask = 0;
				memoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

				sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			}
			else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
			{
				memoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				memoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

				sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
				destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			}
			else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
			{
				memoryBarrier.srcAccessMask = 0;
				memoryBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

				sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			}

			vkCmdPipelineBarrier(commandBuffer->GetCommandBuffer(), sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &memoryBarrier);

			commandBuffer->EndRecording();

		}
	}

}
