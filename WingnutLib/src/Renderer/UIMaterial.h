#pragma once

#include "Material.h"


namespace Wingnut
{


	struct UIMaterialData
	{
		MaterialTexture Texture;
	};


	class UIMaterial : public Material
	{
	public:
		static Ref<UIMaterial> Create(const std::string& name, Ref<Vulkan::Shader> shader);

		UIMaterial(const std::string& name, Ref<Vulkan::Shader> shader);
		virtual ~UIMaterial();

		virtual void SetTexture(MaterialTextureType type, Ref<Vulkan::Texture2D> texture) override;

		virtual void CreateDescriptor(Ref<Vulkan::Shader> shader) override;

		UIMaterialData& GetMaterialData() { return m_MaterialData; }

	protected:
		UIMaterialData m_MaterialData;

		SamplerType m_SamplerType = SamplerType::LinearRepeat;

		Ref<Vulkan::Shader> m_Shader = nullptr;

	private:

	};


}
