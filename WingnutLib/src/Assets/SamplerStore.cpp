#include "wingnut_pch.h"
#include "SamplerStore.h"



namespace Wingnut
{

	std::string SamplerTypeToString(SamplerType type)
	{
		switch (type)
		{
			case SamplerType::Default: return "Default";
			case SamplerType::LinearClamp: return "LinearClamp";
			case SamplerType::LinearRepeat: return "LinearRepeat";
			case SamplerType::Nearest: return "Nearest";
		}

		return "<unknown>";
	}


	void SamplerStore::AddSampler(SamplerType type, Ref<Vulkan::ImageSampler> sampler)
	{
		if (s_Samplers.find(type) != s_Samplers.end())
		{
			LOG_CORE_ERROR("[SamplerStore] Sampler {} already set in SamplerStore", SamplerTypeToString(type));
			return;
		}

		s_Samplers[type] = sampler;
	}

	Ref<Vulkan::ImageSampler> SamplerStore::GetSampler(SamplerType type)
	{
		if (type == SamplerType::Default)
			type = SamplerType::LinearRepeat;

		if (s_Samplers.find(type) != s_Samplers.end())
		{
			return s_Samplers[type];
		}

		return nullptr;
	}

	void SamplerStore::Release()
	{
//		for (auto& shader : s_Shaders)
//		{
//			shader.second->Release();
//		}
	}



}
