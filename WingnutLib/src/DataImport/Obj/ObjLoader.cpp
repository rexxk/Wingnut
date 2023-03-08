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
		std::string objectName = "";

		size_t location = filename.find_last_of("/\\");

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

		uint32_t meshMaterialCount = 0;

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
					meshMaterialCount = 0;
				}

				ss >> objectName;
				
				newObjMesh.ObjectName = objectName;

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

					CalculateTangentAndBitangent(newObjMesh, index, index + 1, index + 2);

					index += 3;
				}
				else if (triangleCount == 4)
				{
					newObjMesh.IndexList.emplace_back(index);
					newObjMesh.IndexList.emplace_back(index + 1);
					newObjMesh.IndexList.emplace_back(index + 2);

					CalculateTangentAndBitangent(newObjMesh, index, index + 1, index + 2);

					newObjMesh.IndexList.emplace_back(index + 2);
					newObjMesh.IndexList.emplace_back(index + 3);
					newObjMesh.IndexList.emplace_back(index);

					CalculateTangentAndBitangent(newObjMesh, index + 2, index + 3, index + 0);

					index += 4;
				}

				// Calculate tangent and bitangent

			}

			if (command == "usemtl")
			{
				if (meshMaterialCount >= 1)
				{
					importResult.Meshes.emplace_back(newObjMesh);

					newObjMesh = ObjMesh();
					newObjMesh.ObjectName = objectName + "_" + std::to_string(meshMaterialCount);

					index = 0;
				}

				ss >> newObjMesh.MaterialName;
				LOG_CORE_TRACE(" - Use material {}", newObjMesh.MaterialName);

				meshMaterialCount++;
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

			// PBR extensions

			if (command == "Pr")
			{
				ss >> newMaterial.Roughness;
				newMaterial.HasPBRValues = true;
			}

			if (command == "Pm")
			{
				ss >> newMaterial.Metalness;
				newMaterial.HasPBRValues = true;
			}


			// Texture maps

			if (command == "map_Ka")
			{
				newMaterial.HasAmbientTexture = true;
				ss >> newMaterial.AmbientTexture;
			}

			if (command == "map_Kd")
			{
				newMaterial.HasDiffuseTexture = true;

				std::string parameter;

				while (newMaterial.DiffuseTexture == "")
				{
					ss >> parameter;

					if (parameter == "-o" || parameter == "-s")
					{
						float x, y, z;
						ss >> x >> y >> z;

						if (parameter == "-o")
						{
							newMaterial.OriginOffset = glm::vec3(x, y, z);
						}
						if (parameter == "-s")
						{
							newMaterial.Scale = glm::vec3(x, y, z);
						}
					}
					else
					{
						newMaterial.DiffuseTexture = parameter;
					}

				}

				newMaterial.DiffuseTexture = parameter;
			}

			if (command == "map_Ks")
			{
				newMaterial.HasMetalnessMap = true;

				std::string parameter;

				while (newMaterial.MetalnessMap == "")
				{
					ss >> parameter;

					if (parameter == "-o" || parameter == "-s")
					{
						float x, y, z;
						ss >> x >> y >> z;

						if (parameter == "-o")
						{
							newMaterial.OriginOffset = glm::vec3(x, y, z);
						}
						if (parameter == "-s")
						{
							newMaterial.Scale = glm::vec3(x, y, z);
						}
					}
					else
					{
						newMaterial.MetalnessMap = parameter;
					}

				}

				newMaterial.MetalnessMap = parameter;
			}

			if (command == "map_Pr")
			{
				newMaterial.HasRoughnessMap = true;

				std::string parameter;

				while (newMaterial.RoughnessMap == "")
				{
					ss >> parameter;

					if (parameter == "-o" || parameter == "-s")
					{
						float x, y, z;
						ss >> x >> y >> z;

						if (parameter == "-o")
						{
							newMaterial.OriginOffset = glm::vec3(x, y, z);
						}
						if (parameter == "-s")
						{
							newMaterial.Scale = glm::vec3(x, y, z);
						}
					}
					else
					{
						newMaterial.RoughnessMap = parameter;
					}

				}

				newMaterial.RoughnessMap = parameter;
			}

			if (command == "map_Bump")
			{
				newMaterial.HasNormalMap = true;

				std::string parameter;

				while (newMaterial.NormalMap == "")
				{
					ss >> parameter;
					float x, y, z;

					if (parameter == "-bm" || parameter == "-o" || parameter == "-s")
					{
						if (parameter == "-o")
						{
							ss >> x >> y >> z;
							newMaterial.OriginOffset = glm::vec3(x, y, z);
						}
						if (parameter == "-s")
						{
							ss >> x >> y >> z;
							newMaterial.Scale = glm::vec3(x, y, z);
						}
						if (parameter == "-bm")
						{
							ss >> x;
						}
					}
					else
					{
						newMaterial.NormalMap = parameter;
					}

				}

				newMaterial.NormalMap = parameter;

			}
		}

		file.close();

		importResult.Materials.emplace_back(newMaterial);
	}

	void ObjLoader::CalculateTangentAndBitangent(ObjMesh& mesh, uint32_t index0, uint32_t index1, uint32_t index2)
	{
		Vertex& v1 = mesh.VertexList[index0];
		Vertex& v2 = mesh.VertexList[index1];
		Vertex& v3 = mesh.VertexList[index2];

		glm::vec3 edge1 = v2.Position - v1.Position;
		glm::vec3 edge2 = v3.Position - v1.Position;
		glm::vec2 dUV1 = v2.TexCoord - v1.TexCoord;
		glm::vec2 dUV2 = v3.TexCoord - v1.TexCoord;

		float f = 1.0f / (dUV1.x * dUV2.y - dUV2.x * dUV1.y);

		glm::vec3 tangent;
		tangent.x = f * (dUV2.y * edge1.x - dUV1.y * edge2.x);
		tangent.y = f * (dUV2.y * edge1.y - dUV1.y * edge2.y);
		tangent.z = f * (dUV2.y * edge1.z - dUV1.y * edge2.z);

		glm::vec3 bitangent;
		bitangent.x = f * (-dUV2.x * edge1.x + dUV1.x * edge2.x);
		bitangent.y = f * (-dUV2.x * edge1.y + dUV1.x * edge2.y);
		bitangent.z = f * (-dUV2.x * edge1.z + dUV1.x * edge2.z);

		v1.Tangent = v2.Tangent = v3.Tangent = tangent;
		v1.Bitangent = v2.Bitangent = v3.Bitangent = bitangent;
	}

}
