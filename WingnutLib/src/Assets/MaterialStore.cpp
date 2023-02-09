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

	UUID MaterialStore::StoreMaterial(Ref<Material> material)
	{
		UUID newMaterialID = UUID();

		s_Materials[newMaterialID] = material;

		return newMaterialID;
	}

	void MaterialStore::ClearMaterials()
	{
		for (auto& material : s_Materials)
		{
			material.second->Release();
		}

		s_Materials.clear();
	}

}
