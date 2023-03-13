#include "wingnut_pch.h"
#include "PBRMaterial.h"

#include "Assets/ResourceManager.h"


namespace Wingnut
{

	Ref<Material> PBRMaterial::Create(const ObjMaterial& objMaterial, Ref<Vulkan::Shader> shader, Ref<Vulkan::ImageSampler> sampler)
	{
		return CreateRef<PBRMaterial>(objMaterial, shader, sampler);
	}

	Ref<Material> PBRMaterial::Create(const std::string& name, Ref<Vulkan::Shader> shader)
	{
		return CreateRef<PBRMaterial>(name, shader);
	}

	PBRMaterial::PBRMaterial(const ObjMaterial& objMaterial, Ref<Vulkan::Shader> shader, Ref<Vulkan::ImageSampler> sampler)
		: Material(objMaterial.MaterialName, shader), m_Shader(shader)
	{
		CreateUniformBuffer();

		m_Sampler = sampler;

		Ref<Vulkan::Texture2D> defaultTexture = Renderer::GetContext()->GetRendererData().DefaultTexture;

		m_MaterialData.AlbedoTexture.Texture = defaultTexture;
		m_MaterialData.NormalMap.Texture = defaultTexture;
		m_MaterialData.MetalnessMap.Texture = defaultTexture;
		m_MaterialData.RoughnessMap.Texture = defaultTexture;
		m_MaterialData.AmbientOcclusionMap.Texture = defaultTexture;

		if (objMaterial.HasDiffuseTexture)
		{
			bool flipTexture = true;

			if (objMaterial.DiffuseTexture.Scale.y == -1)
			{
				flipTexture = false;
			}

			m_MaterialData.AlbedoTexture.Texture = Vulkan::Texture2D::Create(objMaterial.DiffuseTexture.TextureName, Vulkan::TextureFormat::R8G8B8A8_Normalized, flipTexture, true);

			ResourceManager::AddTexture(m_MaterialData.AlbedoTexture.Texture);

//			Ref<Vulkan::Descriptor> uiTextureDescriptor = Vulkan::Descriptor::Create(Renderer::GetContext()->GetRendererData().Device, ResourceManager::GetShader(ShaderType::ImGui), ImGuiTextureDescriptor);

//			uiTextureDescriptor->SetImageBinding(ImGuiTextureBinding, m_MaterialData.AlbedoTexture.Texture, sampler);
//			uiTextureDescriptor->UpdateBindings();

//			ResourceManager::AddTextureData(m_MaterialData.AlbedoTexture.Texture, uiTextureDescriptor);

			m_MaterialData.Properties.UseAlbedoTexture = true;
		}

		if (objMaterial.HasNormalMap)
		{
			bool flipTexture = true;

			if (objMaterial.NormalMap.Scale.y == -1)
			{
				flipTexture = false;
			}

			m_MaterialData.NormalMap.Texture = Vulkan::Texture2D::Create(objMaterial.NormalMap.TextureName, Vulkan::TextureFormat::R8G8B8A8_Normalized, flipTexture, true);

			ResourceManager::AddTexture(m_MaterialData.NormalMap.Texture);

//			Ref<Vulkan::Descriptor> uiTextureDescriptor = Vulkan::Descriptor::Create(Renderer::GetContext()->GetRendererData().Device, ResourceManager::GetShader(ShaderType::ImGui), ImGuiTextureDescriptor);

//			uiTextureDescriptor->SetImageBinding(ImGuiTextureBinding, m_MaterialData.NormalMap.Texture, sampler);
//			uiTextureDescriptor->UpdateBindings();

//			ResourceManager::AddTextureData(m_MaterialData.NormalMap.Texture, uiTextureDescriptor);

			m_MaterialData.Properties.UseNormalMap = true;
		}

		if (objMaterial.HasMetalnessMap)
		{
			bool flipTexture = true;

			if (objMaterial.MetalnessMap.Scale.y == -1)
			{
				flipTexture = false;
			}

			m_MaterialData.MetalnessMap.Texture = Vulkan::Texture2D::Create(objMaterial.MetalnessMap.TextureName, Vulkan::TextureFormat::R8G8B8A8_Normalized, flipTexture, true);

			ResourceManager::AddTexture(m_MaterialData.MetalnessMap.Texture);

//			Ref<Vulkan::Descriptor> uiTextureDescriptor = Vulkan::Descriptor::Create(Renderer::GetContext()->GetRendererData().Device, ResourceManager::GetShader(ShaderType::ImGui), ImGuiTextureDescriptor);

//			uiTextureDescriptor->SetImageBinding(ImGuiTextureBinding, m_MaterialData.MetalnessMap.Texture, sampler);
//			uiTextureDescriptor->UpdateBindings();

//			ResourceManager::AddTextureData(m_MaterialData.MetalnessMap.Texture, uiTextureDescriptor);

			m_MaterialData.Properties.UseMetalnessMap = true;
		}

		if (objMaterial.HasRoughnessMap)
		{
			bool flipTexture = true;

			if (objMaterial.RoughnessMap.Scale.y == -1)
			{
				flipTexture = false;
			}

			m_MaterialData.RoughnessMap.Texture = Vulkan::Texture2D::Create(objMaterial.RoughnessMap.TextureName, Vulkan::TextureFormat::R8G8B8A8_Normalized, flipTexture, true);

			ResourceManager::AddTexture(m_MaterialData.RoughnessMap.Texture);

//			Ref<Vulkan::Descriptor> uiTextureDescriptor = Vulkan::Descriptor::Create(Renderer::GetContext()->GetRendererData().Device, ResourceManager::GetShader(ShaderType::ImGui), ImGuiTextureDescriptor);

//			uiTextureDescriptor->SetImageBinding(ImGuiTextureBinding, m_MaterialData.RoughnessMap.Texture, sampler);
//			uiTextureDescriptor->UpdateBindings();

//			ResourceManager::AddTextureData(m_MaterialData.RoughnessMap.Texture, uiTextureDescriptor);

			m_MaterialData.Properties.UseRoughnessMap = true;
		}

		if (objMaterial.HasAmbientOcclusionMap)
		{
			bool flipTexture = true;

			if (objMaterial.AmbientOcclusionMap.Scale.y == -1)
			{
				flipTexture = false;
			}

			m_MaterialData.AmbientOcclusionMap.Texture = Vulkan::Texture2D::Create(objMaterial.AmbientOcclusionMap.TextureName, Vulkan::TextureFormat::R8G8B8A8_Normalized, flipTexture, true);

			ResourceManager::AddTexture(m_MaterialData.AmbientOcclusionMap.Texture);

//			Ref<Vulkan::Descriptor> uiTextureDescriptor = Vulkan::Descriptor::Create(Renderer::GetContext()->GetRendererData().Device, ResourceManager::GetShader(ShaderType::ImGui), ImGuiTextureDescriptor);

//			uiTextureDescriptor->SetImageBinding(ImGuiTextureBinding, m_MaterialData.AmbientOcclusionMap.Texture, sampler);
//			uiTextureDescriptor->UpdateBindings();

//			ResourceManager::AddTextureData(m_MaterialData.AmbientOcclusionMap.Texture, uiTextureDescriptor);

			m_MaterialData.Properties.UseAmbientOcclusionMap = true;
		}

		if (objMaterial.HasPBRValues)
		{
			m_MaterialData.Properties.Metallic = objMaterial.Metalness;
			m_MaterialData.Properties.Roughness = objMaterial.Roughness;
			m_MaterialData.Properties.AmbientOcclusion = objMaterial.AmbientOcclusion;
		}
		else
		{
			m_MaterialData.Properties.Metallic = 0.04f;
			m_MaterialData.Properties.Roughness = 1.0f;
			m_MaterialData.Properties.AmbientOcclusion = 1.0f;
		}

		m_MaterialData.Properties.AlbedoColor = glm::vec4(objMaterial.Diffuse, objMaterial.Transparency);

		CreateDescriptor(m_Shader);

		Update();
	}

	PBRMaterial::PBRMaterial(const std::string& name, Ref<Vulkan::Shader> shader)
		: Material(name, shader)
	{
		CreateUniformBuffer();

		m_MaterialData.Properties.AlbedoColor = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
		m_MaterialData.Properties.UseAlbedoTexture = true;

		m_MaterialData.AlbedoTexture.Texture = Renderer::GetContext()->GetRendererData().DefaultTexture;
		m_MaterialData.NormalMap.Texture = Renderer::GetContext()->GetRendererData().DefaultTexture;
		m_MaterialData.MetalnessMap.Texture = Renderer::GetContext()->GetRendererData().DefaultTexture;
		m_MaterialData.RoughnessMap.Texture = Renderer::GetContext()->GetRendererData().DefaultTexture;
		m_MaterialData.AmbientOcclusionMap.Texture = Renderer::GetContext()->GetRendererData().DefaultTexture;


		//		m_Descriptor = Vulkan::Descriptor::Create(Renderer::GetContext()->GetRendererData().Device, m_Shader, MaterialDescriptor);

		CreateDescriptor(shader);

		//		m_Descriptor->SetBufferBinding(MaterialDataBinding, m_MaterialUB);
		//		m_Descriptor->SetImageBinding(AlbedoTextureBinding, m_MaterialData.AlbedoTexture.Texture, SamplerStore::GetSampler(SamplerType::Default));

		Update();
	}

	PBRMaterial::~PBRMaterial()
	{
		Release();
	}

	void PBRMaterial::Release()
	{
		if (m_MaterialUB != nullptr)
		{
			m_MaterialUB->Release();
		}
	}

	void PBRMaterial::Update()
	{
		m_MaterialUB->Update(&m_MaterialData.Properties, sizeof(PBRMaterialProperties), Renderer::GetContext()->GetCurrentFrame());
	}

	void PBRMaterial::CreateUniformBuffer()
	{
		m_MaterialUB = Vulkan::UniformBuffer::Create(Renderer::GetContext()->GetRendererData().Device, sizeof(PBRMaterialProperties));
	}

	void PBRMaterial::CreateDescriptor(Ref<Vulkan::Shader> shader)
	{
		auto& rendererData = Renderer::GetContext()->GetRendererData();
		m_Descriptor = Vulkan::Descriptor::Create(rendererData.Device, shader, MaterialDescriptor);

		m_Descriptor->SetBufferBinding(PBRMaterialDataBinding, m_MaterialUB);
		m_Descriptor->SetImageBinding(PBRAlbedoTextureBinding, m_MaterialData.AlbedoTexture.Texture, m_Sampler);
		m_Descriptor->SetImageBinding(PBRNormalMapBinding, m_MaterialData.NormalMap.Texture, m_Sampler);
		m_Descriptor->SetImageBinding(PBRMetalnessMapBinding, m_MaterialData.MetalnessMap.Texture, m_Sampler);
		m_Descriptor->SetImageBinding(PBRRoughnessMapBinding, m_MaterialData.RoughnessMap.Texture, m_Sampler);
		m_Descriptor->SetImageBinding(PBRAmbientOcclusionMapBinding, m_MaterialData.AmbientOcclusionMap.Texture, m_Sampler);

		m_Descriptor->UpdateBindings();
	}

	void PBRMaterial::SetTexture(MaterialTextureType type, Ref<Vulkan::Texture2D> texture)
	{
		switch (type)
		{
		case MaterialTextureType::AlbedoTexture:
		{
			m_MaterialData.AlbedoTexture.Texture = texture;
			m_Descriptor->SetImageBinding(PBRAlbedoTextureBinding, m_MaterialData.AlbedoTexture.Texture, m_Sampler);

			break;
		}

		case MaterialTextureType::NormalMap:
		{
			m_MaterialData.NormalMap.Texture = texture;
			m_Descriptor->SetImageBinding(PBRNormalMapBinding, m_MaterialData.NormalMap.Texture, m_Sampler);

			break;
		}

		case MaterialTextureType::MetalnessMap:
		{
			m_MaterialData.MetalnessMap.Texture = texture;
			m_Descriptor->SetImageBinding(PBRMetalnessMapBinding, m_MaterialData.MetalnessMap.Texture, m_Sampler);

			break;
		}

		case MaterialTextureType::RoughnessMap:
		{
			m_MaterialData.RoughnessMap.Texture = texture;
			m_Descriptor->SetImageBinding(PBRRoughnessMapBinding, m_MaterialData.RoughnessMap.Texture, m_Sampler);

			break;
		}

		case MaterialTextureType::AmbientOcclusionMap:
		{
			m_MaterialData.AmbientOcclusionMap.Texture = texture;
			m_Descriptor->SetImageBinding(PBRAmbientOcclusionMapBinding, m_MaterialData.AmbientOcclusionMap.Texture, m_Sampler);

			break;
		}
		}


		m_Descriptor->UpdateBindings();

	}


}
