#include "wingnut_pch.h"
#include "UIMaterial.h"

#include "Assets/ResourceManager.h"


namespace Wingnut
{

	Ref<Material> UIMaterial::Create(const std::string& name, Ref<Vulkan::Shader> shader)
	{
		return CreateRef<UIMaterial>(name, shader);
	}


	UIMaterial::UIMaterial(const std::string& name, Ref<Vulkan::Shader> shader)
		: Material(name, shader)
	{
		m_MaterialType = MaterialType::UI;

		m_MaterialData.Texture.Texture = Renderer::GetContext()->GetRendererData().DefaultTexture;
		m_Sampler = ResourceManager::GetSampler(SamplerType::Default);

		CreateDescriptor(shader);

		Update();
	}

	UIMaterial::~UIMaterial()
	{
		Release();
	}

	void UIMaterial::SetTexture(MaterialTextureType type, Ref<Vulkan::Texture2D> texture)
	{
		m_MaterialData.Texture.Texture = texture;
		m_Descriptor->SetImageBinding(ImGuiTextureBinding, m_MaterialData.Texture.Texture, m_Sampler);

		m_Descriptor->UpdateBindings();

	}

	void UIMaterial::CreateDescriptor(Ref<Vulkan::Shader> shader)
	{
		auto& rendererData = Renderer::GetContext()->GetRendererData();
		m_Descriptor = Vulkan::Descriptor::Create(rendererData.Device, shader, ImGuiTextureDescriptor);

		m_Descriptor->SetImageBinding(ImGuiTextureBinding, m_MaterialData.Texture.Texture, m_Sampler);
		m_Descriptor->UpdateBindings();
	}

}
