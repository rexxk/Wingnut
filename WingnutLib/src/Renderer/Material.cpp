#include "wingnut_pch.h"
#include "Material.h"

#include "Assets/ResourceManager.h"

#include "Renderer/Renderer.h"

#include "Platform/Vulkan/Buffer.h"
#include "Platform/Vulkan/Shader.h"


namespace Wingnut
{

	Ref<Material> Material::Create(const ObjMaterial& objMaterial, Ref<Vulkan::Shader> shader, Ref<Vulkan::ImageSampler> sampler)
	{
		return CreateRef<Material>(objMaterial, shader, sampler);
	}

	Ref<Material> Material::Create(const std::string& name, Ref<Vulkan::Shader> shader)
	{
		return CreateRef<Material>(name, shader);
	}


	Material::Material(const std::string& name, Ref<Vulkan::Shader> shader)
		: m_Name(name), m_Shader(shader)
	{

	}

	Material::~Material()
	{
		Release();
	}

	void Material::Release()
	{
	
	}

	void Material::Update()
	{

	}

	void Material::CreateDescriptor(Ref<Vulkan::Shader> shader)
	{

	}

	void Material::SetTexture(MaterialTextureType type, Ref<Vulkan::Texture2D> texture)
	{

	}

}
