#pragma once

#include "Platform/Vulkan/Image.h"



namespace Wingnut
{

	enum class SamplerType
	{
		Default = 0,
		LinearRepeat,
		LinearClamp,
		NearestRepeat,
	};



	class SamplerStore
	{
	public:

		static std::string SamplerTypeToString(SamplerType type);
		static SamplerType GetSamplerTypeByName(const std::string& samplerName);

		static void AddSampler(SamplerType type, Ref<Vulkan::ImageSampler> sampler);

		static Ref<Vulkan::ImageSampler> GetSampler(SamplerType type);

		static void Release();

		static std::unordered_map<SamplerType, Ref<Vulkan::ImageSampler>> GetSamplerMap() { return s_Samplers; }

	private:
		static inline std::unordered_map<SamplerType, Ref<Vulkan::ImageSampler>> s_Samplers;
	};

}
