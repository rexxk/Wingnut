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

		if (str[newPosition] != '/')
		{
			location = str.find('/', location + 1);
			texCoordIndex = std::stoi(str.substr(newPosition, location - newPosition));

			newPosition = location + 1;
		}
		else
		{
			texCoordIndex = 0;
			newPosition++;
		}

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

		ObjMesh newObjMesh;

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
				// Start reading a new object

				if (newObjMesh.IndexList.size() > 0)
				{
					importResult.Meshes.emplace_back(newObjMesh);

					newObjMesh = ObjMesh();

					index = 0;
				}

				ss >> newObjMesh.ObjectName;

				LOG_CORE_TRACE("[ObjLoader] Importing object {}", newObjMesh.ObjectName);
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

					if (indexVector.y > 0)
						newVertex.TexCoord = texCoords[indexVector.y - 1];

					newVertex.Normal = normals[indexVector.z - 1];
					newVertex.Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

					newObjMesh.VertexList.emplace_back(newVertex);
				}

				if (triangleCount == 3)
				{
					newObjMesh.IndexList.emplace_back(index);
					newObjMesh.IndexList.emplace_back(index + 1);
					newObjMesh.IndexList.emplace_back(index + 2);

					index += 3;
				}
				else if (triangleCount == 4)
				{
					newObjMesh.IndexList.emplace_back(index);
					newObjMesh.IndexList.emplace_back(index + 1);
					newObjMesh.IndexList.emplace_back(index + 2);

					newObjMesh.IndexList.emplace_back(index + 2);
					newObjMesh.IndexList.emplace_back(index + 3);
					newObjMesh.IndexList.emplace_back(index);

					index += 4;
				}
			}

			if (command == "usemtl")
			{
				ss >> newObjMesh.MaterialName;
			}
		}

		file.close();

		if (materialFilename != "")
		{
			LoadMaterialFile(filePath + materialFilename, importResult);
		}

		importResult.HasMeshData = true;

		importResult.Meshes.emplace_back(newObjMesh);


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

		ObjMaterial newMaterial;

		while (!file.eof())
		{
			std::string line;
			std::getline(file, line);

			std::stringstream ss(line);
			std::string command;

			ss >> command;

			if (command == "newmtl")
			{
				if (newMaterial.MaterialName != "")
				{
					importResult.Materials.emplace_back(newMaterial);
					importResult.HasMaterial = true;

				}

				newMaterial = ObjMaterial();
				ss >> newMaterial.MaterialName;

			}

			// Ambient
			if (command == "Ka")
			{
				ss >> newMaterial.Ambient.r >> newMaterial.Ambient.g >> newMaterial.Ambient.b;
			}

			// Diffuse
			if (command == "Kd")
			{
				ss >> newMaterial.Diffuse.r >> newMaterial.Diffuse.g >> newMaterial.Diffuse.b;
			}

			// Specular
			if (command == "Ks")
			{
				ss >> newMaterial.Specular.r >> newMaterial.Specular.g >> newMaterial.Specular.b;
			}

			// Dissolve (transparency)
			if (command == "d")
			{
				ss >> newMaterial.Transparency;
			}

			// Optical density (index of refraction)
			if (command == "Ni")
			{
				ss >> newMaterial.OpticalDensity;
			}

			if (command == "map_Ka")
			{
				newMaterial.HasAmbientTexture = true;
				ss >> newMaterial.AmbientTexture;
			}

			if (command == "map_Kd")
			{
				newMaterial.HasDiffuseTexture = true;
				ss >> newMaterial.DiffuseTexture;
			}

			if (command == "map_Ks")
			{
				newMaterial.HasSpecularTexture = true;
				ss >> newMaterial.SpecularTexture;
			}

		}

		file.close();

		importResult.Materials.emplace_back(newMaterial);
	}


}
