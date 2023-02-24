#pragma once

#include "Renderer/Renderer.h"


namespace Wingnut
{


	struct ObjImportResult
	{
		std::string ObjectName;

		std::vector<Vertex> VertexList;
		std::vector<uint32_t> IndexList;

		// Material data

		std::string MaterialName;

		glm::vec3 Ambient;
		glm::vec3 Diffuse;
		glm::vec3 Specular;

		float Transparency;
		float OpticalDensity;

		// Texture paths
		std::string AmbientTexture;
		std::string DiffuseTexture;
		std::string SpecularTexture;
	};


	class ObjLoader
	{
	public:
		static ObjImportResult Import(const std::string& filename);

	private:
		static void LoadMaterialFile(const std::string& filename, ObjImportResult& importResult);
	};


}
