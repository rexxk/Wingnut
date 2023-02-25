#include "wingnut_pch.h"
#include "ObjLoader.h"

#include <glm/glm.hpp>


namespace Wingnut
{

	glm::ivec3 GetIndexValuesFromString(const std::string& str)
	{
		int positionIndex = 0;
		int texCoordIndex = 0;
		int normalIndex = 0;

		size_t location = str.find('/');
		positionIndex = std::stoi(str.substr(0, location));

		size_t newPosition = location + 1;

		location = str.find('/', location + 1);
		texCoordIndex = std::stoi(str.substr(newPosition, location - newPosition));

		newPosition = location + 1;

		normalIndex = std::stoi(str.substr(newPosition));

		return glm::ivec3(positionIndex, texCoordIndex, normalIndex);
	}



	ObjImportResult ObjLoader::Import(const std::string& filename)
	{
		std::string filePath;
		std::string materialFilename = "";

		size_t location = filename.find_last_of('/');

		if (location != std::string::npos)
		{
			filePath = filename.substr(0, location + 1);
		}

		LOG_CORE_TRACE("[ObjLoader] Filepath: {}", filePath);

		std::ifstream file(filename, std::ios::in | std::ios::binary);

		ObjImportResult importResult;

		if (!file.is_open())
		{
			LOG_CORE_ERROR("[ObjLoader] Unable to import file {}", filename);
			return importResult;
		}

		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> texCoords;
		std::vector<glm::vec3> normals;

		std::vector<Vertex> vertexList;
		std::vector<uint32_t> indexList;

		uint32_t index = 0;

		while (!file.eof())
		{
			std::string line;
			std::getline(file, line);

			std::stringstream ss(line);
			std::string command;

			float x, y, z;

			ss >> command;

			if (command == "mtllib")
			{
				ss >> materialFilename;
			}

			if (command == "o")
			{
				ss >> importResult.ObjectName;
			}

			if (command == "v")
			{
				ss >> x >> y >> z;

				positions.emplace_back(glm::vec3(x, y, z));
			}

			if (command == "vt")
			{
				ss >> x >> y;

				texCoords.emplace_back(glm::vec2(x, y));
			}

			if (command == "vn")
			{
				ss >> x >> y >> z;

				normals.emplace_back(glm::vec3(x, y, z));
			}

			if (command == "f")
			{
				std::string s[4];

				ss >> s[0] >> s[1] >> s[2] >> s[3];

				uint32_t triangleCount = 3;

				if (s[3] != "")
				{
					triangleCount = 4;
				}

				for (uint32_t i = 0; i < triangleCount; i++)
				{
					glm::ivec3 indexVector = GetIndexValuesFromString(s[i]);

					Vertex newVertex;
					newVertex.Position = positions[indexVector.x - 1];
					newVertex.TexCoord = texCoords[indexVector.y - 1];
					newVertex.Normal = normals[indexVector.z - 1];
					newVertex.Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

					vertexList.emplace_back(newVertex);
				}

				if (triangleCount == 3)
				{
					indexList.emplace_back(index);
					indexList.emplace_back(index + 1);
					indexList.emplace_back(index + 2);

					index += 3;
				}
				else if (triangleCount == 4)
				{
					indexList.emplace_back(index);
					indexList.emplace_back(index + 1);
					indexList.emplace_back(index + 2);

					indexList.emplace_back(index + 2);
					indexList.emplace_back(index + 3);
					indexList.emplace_back(index);

					index += 4;
				}
			}
		}

		file.close();

		if (materialFilename != "")
		{
			LoadMaterialFile(filePath + materialFilename, importResult);
		}

		importResult.HasMeshData = true;

		importResult.VertexList = vertexList;
		importResult.IndexList = indexList;

		return importResult;
	}

	void ObjLoader::LoadMaterialFile(const std::string& filename, ObjImportResult& importResult)
	{
		std::ifstream file(filename, std::ios::in | std::ios::binary);

		if (!file.is_open())
		{
			LOG_CORE_ERROR("[ObjLoader] Unable to open material file {}", filename);
			return;
		}

		while (!file.eof())
		{
			std::string line;
			std::getline(file, line);

			std::stringstream ss(line);
			std::string command;

			ss >> command;

			if (command == "newmtl")
			{
				ss >> importResult.Material.MaterialName;

				importResult.HasMaterial = true;
			}

			// Ambient
			if (command == "Ka")
			{
				ss >> importResult.Material.Ambient.r >> importResult.Material.Ambient.g >> importResult.Material.Ambient.b;
			}

			// Diffuse
			if (command == "Kd")
			{
				ss >> importResult.Material.Diffuse.r >> importResult.Material.Diffuse.g >> importResult.Material.Diffuse.b;
			}

			// Specular
			if (command == "Ks")
			{
				ss >> importResult.Material.Specular.r >> importResult.Material.Specular.g >> importResult.Material.Specular.b;
			}

			// Dissolve (transparency)
			if (command == "d")
			{
				ss >> importResult.Material.Transparency;
			}

			// Optical density (index of refraction)
			if (command == "Ni")
			{
				ss >> importResult.Material.OpticalDensity;
			}

			if (command == "map_Ka")
			{
				importResult.Material.HasAmbientTexture = true;
				ss >> importResult.Material.AmbientTexture;
			}

			if (command == "map_Kd")
			{
				importResult.Material.HasDiffuseTexture = true;
				ss >> importResult.Material.DiffuseTexture;
			}

			if (command == "map_Ks")
			{
				importResult.Material.HasSpecularTexture = true;
				ss >> importResult.Material.SpecularTexture;
			}

		}

		file.close();
	}


}
