#pragma once



namespace Wingnut
{


	class Material;


	class MaterialStore
	{
	public:

		static void LoadMaterial(const std::string& materialPath);

		static Ref<Material> GetMaterial(UUID materialID);
		static UUID StoreMaterial(Ref<Material> material);

		static void ClearMaterials();

	private:
		inline static std::unordered_map<UUID, Ref<Material>> s_Materials;
	};


}
