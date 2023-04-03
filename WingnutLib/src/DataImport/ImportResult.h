#pragma once

#include "wingnut_pch.h"

#include "Renderer/Renderer.h"

#include <glm/glm.hpp>


namespace Wingnut
{

	struct ImportMesh
	{
		std::string ObjectName = "";

		std::vector<Vertex> VertexList;
		std::vector<uint32_t> IndexList;

		glm::mat4 Transform;

		std::string MaterialName = "material";
	};

	struct ImportTexture
	{
		std::string TextureName = "";

		glm::vec3 OriginOffset = glm::vec3(0.0f);
		glm::vec3 Scale = glm::vec3(1.0f);

		float BumpMultiplier = 1.0f;
	};

	struct ImportMaterial
	{
		std::string MaterialName = "";

		glm::vec3 Ambient;
		glm::vec3 Diffuse;
		glm::vec3 Specular;

		float Transparency = 1.0f;
		float OpticalDensity = 0.0f;

		bool HasPBRValues = false;

		float Roughness = 1.0f;
		float Metalness = 0.04f;
		float AmbientOcclusion = 1.0f;


		// Texture paths
		bool HasDiffuseTexture = false;
		ImportTexture DiffuseTexture;
		bool HasNormalMap = false;
		ImportTexture NormalMap;
		bool HasMetalnessMap = false;
		ImportTexture MetalnessMap;
		bool HasRoughnessMap = false;
		ImportTexture RoughnessMap;
		bool HasAmbientOcclusionMap = false;
		ImportTexture AmbientOcclusionMap;
	};

	struct ImportResult
	{
		bool HasMeshData = false;
		std::vector<ImportMesh> Meshes;

		// Material data
		bool HasMaterial = false;
		std::vector<ImportMaterial> Materials;

		std::string ModelName;
	};


}


