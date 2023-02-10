#include "wingnut_pch.h"
#include "MaterialStore.h"

#include "Renderer/Material.h"


namespace Wingnut
{

	void MaterialStore::LoadMaterial(const std::string& materialPath)
	{
		LOG_CORE_ERROR("[MaterialStore] Loading is not implemented");
	}

	Ref<Material> MaterialStore::GetMaterial(UUID materialID)
	{
		if (s_Materials.find(materialID) != s_Materials.end())
		{
			return s_Materials[materialID];
		}

		return nullptr;
	}

	void MaterialStore::StoreMaterial(Ref<Material> material)
	{
		s_Materials[material->GetID()] = material;
	}

	void MaterialStore::ClearMaterials()
	{
		for (auto& material : s_Materials)
		{
			material.second->Release();
		}

		s_Materials.clear();
	}

	Ref<Material> MaterialStore::GetMaterialByName(const std::string& name)
	{
		for (auto& materialInstance : s_Materials)
		{
			auto& material = materialInstance.second;

			if (material->GetName() == name)
			{
				return material;
			}
		}

		return nullptr;
	}

}
