#pragma once



namespace Wingnut
{


	class Material;


	class MaterialStore
	{
	public:

		static void LoadMaterial(const std::string& materialPath);

		static Ref<Material> GetMaterial(UUID materialID);
		static void StoreMaterial(Ref<Material> material);

		static Ref<Material> GetMaterialByName(const std::string& name);

		static void ClearMaterials();

		static void DeleteMaterial(UUID materialID);

		static const std::unordered_map<UUID, Ref<Material>>& GetMaterialList() { return s_Materials; }

	private:
		inline static std::unordered_map<UUID, Ref<Material>> s_Materials;
	};


}
