#include "wingnut_pch.h"
#include "SamplerStore.h"



namespace Wingnut
{

	std::string SamplerStore::SamplerTypeToString(SamplerType type)
	{
		switch (type)
		{
			case SamplerType::Default: return "Default";
			case SamplerType::LinearClamp: return "LinearClamp";
			case SamplerType::LinearRepeat: return "LinearRepeat";
			case SamplerType::NearestRepeat: return "NearestRepeat";
		}

		return "<unknown>";
	}

	SamplerType SamplerStore::GetSamplerTypeByName(const std::string& samplerName)
	{
		if (samplerName == "Default") return SamplerType::Default;
		if (samplerName == "LinearClamp") return SamplerType::LinearClamp;
		if (samplerName == "LinearRepeat") return SamplerType::LinearRepeat;
		if (samplerName == "NearestRepeat") return SamplerType::NearestRepeat;

		return SamplerType::Default;
	}

	void SamplerStore::AddSampler(SamplerType type, Ref<Vulkan::ImageSampler> sampler)
	{
		if (s_Samplers.find(type) != s_Samplers.end())
		{
			LOG_CORE_ERROR("[SamplerStore] Sampler {} already set in SamplerStore", SamplerTypeToString(type));
			return;
		}

		LOG_CORE_TRACE("[SamplerStore] Adding sampler {}", SamplerTypeToString(type));

		s_Samplers[type] = sampler;
	}

	Ref<Vulkan::ImageSampler> SamplerStore::GetSampler(SamplerType type)
	{
		if (type == SamplerType::Default)
			type = SamplerType::NearestRepeat;

		if (s_Samplers.find(type) != s_Samplers.end())
		{
			return s_Samplers[type];
		}

		return nullptr;
	}

	void SamplerStore::Release()
	{
		for (auto& sampler : s_Samplers)
		{
			sampler.second->Release();
		}
	}



}
