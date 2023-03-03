#pragma once

#include "Renderer/Renderer.h"


namespace Wingnut
{

	struct ObjMesh
	{
		std::string ObjectName = "";

		std::vector<Vertex> VertexList;
		std::vector<uint32_t> IndexList;

		std::string MaterialName = "material";
	};

	struct ObjMaterial
	{
		std::string MaterialName = "";

		glm::vec3 Ambient;
		glm::vec3 Diffuse;
		glm::vec3 Specular;

		float Transparency = 1.0f;
		float OpticalDensity = 0.0f;

		glm::vec3 OriginOffset = glm::vec3(0.0f);
		glm::vec3 Scale = glm::vec3(1.0f);

		bool HasPBRValues = false;

		float Roughness = 1.0f;
		float Metalness = 0.04f;
		float AmbientOcclusion = 1.0f;


		// Texture paths
		bool HasAmbientTexture = false;
		std::string AmbientTexture;
		bool HasDiffuseTexture = false;
		std::string DiffuseTexture;

		bool HasNormalMap = false;
		std::string NormalMap;

		bool HasMetalnessMap = false;
		std::string MetalnessMap;
		bool HasRoughnessMap = false;
		std::string RoughnessMap;

		bool HasAmbientOcclusionMap = false;
		std::string AmbientOcclusionMap;
	};

	struct ObjImportResult
	{
		bool HasMeshData = false;
		std::vector<ObjMesh> Meshes;

		// Material data
		bool HasMaterial = false;
		std::vector<ObjMaterial> Materials;
	};


	class ObjLoader
	{
	public:
		static ObjImportResult Import(const std::string& filename);

	private:
		static void CalculateTangentAndBitangent(ObjMesh& mesh, uint32_t index0, uint32_t index1, uint32_t index2);

		static void LoadMaterialFile(const std::string& filename, ObjImportResult& importResult);
	};


}
