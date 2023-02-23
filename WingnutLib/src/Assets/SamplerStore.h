#pragma once

#include "Platform/Vulkan/Image.h"



namespace Wingnut
{

	enum class SamplerType
	{
		Default,
		LinearRepeat,
		LinearClamp,
		Nearest,
	};



	class SamplerStore
	{
	public:
		static void AddSampler(SamplerType type, Ref<Vulkan::ImageSampler> sampler);

		static Ref<Vulkan::ImageSampler> GetSampler(SamplerType type);

		static void Release();

	private:
		static inline std::unordered_map<SamplerType, Ref<Vulkan::ImageSampler>> s_Samplers;
	};

}
