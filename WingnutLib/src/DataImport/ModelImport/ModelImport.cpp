#include "wingnut_pch.h"
#include "ModelImport.h"

#include "assimp/cimport.h"
#include "assimp/Importer.hpp"
#include "assimp/material.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Assets/ResourceManager.h"

#include "Platform/Vulkan/Texture.h"

#include "Utils/StringUtils.h"

#include <glm/glm.hpp>


namespace Wingnut
{


	glm::mat4 aiMatrix4x4ToGlmMat4(const aiMatrix4x4& from)
	{
		glm::mat4 to;

		to[0][0] = (float)from.a1; to[0][1] = (float)from.b1; to[0][2] = (float)from.c1; to[0][3] = (float)from.d1;
		to[1][0] = (float)from.a2; to[1][1] = (float)from.b2; to[1][2] = (float)from.c2; to[1][3] = (float)from.d2;
		to[2][0] = (float)from.a3; to[2][1] = (float)from.b3; to[2][2] = (float)from.c3; to[2][3] = (float)from.d3;
		to[3][0] = (float)from.a4; to[3][1] = (float)from.b4; to[3][2] = (float)from.c4; to[3][3] = (float)from.d4;

		return to;
	}


	ImportResult ModelImport::ImportFBX(const std::string& filepath)
	{

		auto importer = Assimp::Importer();
		importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_READ_ALL_MATERIALS, false);
		const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_CalcTangentSpace | aiProcess_ValidateDataStructure); // | aiProcess_FlipUVs);

		if (!scene || scene->mRootNode == nullptr)
		{
			LOG_CORE_ERROR("[ModelImport] Failed to load model {}", filepath);

			LOG_CORE_ERROR("[ModelImport] {}", aiGetErrorString());

			return ImportResult();
		}

		ImportResult importResult;

		std::string sceneName = scene->mName.C_Str();
		
		if (sceneName == "")
		{
			sceneName = "scene";
		}

		aiNode* rootNode = scene->mRootNode;

		GetNodeData(scene, rootNode, importResult, glm::mat4(1.0f));



		if (scene->HasLights())
		{
			LOG_CORE_WARN("[ModelImport] Have {} lights", scene->mNumLights);
		}

		if (scene->HasMaterials())
		{
			for (uint32_t i = 0; i < scene->mNumMaterials; i++)
			{
				auto* material = scene->mMaterials[i];

				ImportMaterial newMaterial;
				newMaterial.MaterialName = material->GetName().C_Str();

				if (newMaterial.MaterialName == "")
				{
					newMaterial.MaterialName = "Material_" + std::to_string(i);
				}

				LOG_CORE_TRACE("Material found: {}", newMaterial.MaterialName);

				for (uint32_t propertyIndex = 0; propertyIndex < material->mNumProperties; propertyIndex++)
				{
					aiMaterialProperty* property = material->mProperties[propertyIndex];
					LOG_CORE_TRACE(" Property: {}", property->mKey.C_Str());
				}

				if (material->GetTextureCount(aiTextureType_BASE_COLOR) > 0)
				{
					for (uint32_t baseColorIndex = 0; baseColorIndex < material->GetTextureCount(aiTextureType_DIFFUSE); baseColorIndex++)
					{
						aiString textureName;
						material->Get(AI_MATKEY_TEXTURE(aiTextureType_BASE_COLOR, baseColorIndex), textureName);

						LOG_CORE_TRACE(" BASE COLOR filename: {}", textureName.C_Str());

						newMaterial.HasDiffuseTexture = true;
						newMaterial.DiffuseTexture.TextureName = "assets/textures/" + sceneName + "/" + ConvertFilePath(textureName.C_Str());
					}

				}

				if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
				{
					for (uint32_t diffuseIndex = 0; diffuseIndex < material->GetTextureCount(aiTextureType_DIFFUSE); diffuseIndex++)
					{
						aiString textureName;
						material->Get(AI_MATKEY_TEXTURE_DIFFUSE(diffuseIndex), textureName);

						LOG_CORE_TRACE(" DIFFUSE filename: {}", textureName.C_Str());

						newMaterial.HasDiffuseTexture = true;
						newMaterial.DiffuseTexture.TextureName = "assets/textures/" + sceneName + "/" + ConvertFilePath(textureName.C_Str());
					}

				}

				if (material->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION) > 0)
				{
					for (uint32_t textureIndex = 0; textureIndex < material->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION); textureIndex++)
					{
						aiString textureName;
						material->Get(AI_MATKEY_TEXTURE(aiTextureType_AMBIENT_OCCLUSION, textureIndex), textureName);

						LOG_CORE_TRACE(" AMBIENT OCCLUSION filename: {}", textureName.C_Str());

						newMaterial.HasAmbientOcclusionMap = true;
						newMaterial.AmbientOcclusionMap.TextureName = "assets/textures/" + sceneName + "/" + ConvertFilePath(textureName.C_Str());
					}
				}

				if (material->GetTextureCount(aiTextureType_SPECULAR) > 0)
				{
					for (uint32_t specularIndex = 0; specularIndex < material->GetTextureCount(aiTextureType_DIFFUSE); specularIndex++)
					{
						aiString textureName;
						material->Get(AI_MATKEY_TEXTURE_SPECULAR(specularIndex), textureName);

						LOG_CORE_TRACE(" SPECULAR filename: {}", textureName.C_Str());

						newMaterial.HasMetalnessMap = true;
						newMaterial.MetalnessMap.TextureName = "assets/textures/" + sceneName + "/" + ConvertFilePath(textureName.C_Str());
					}
				}

				if (material->GetTextureCount(aiTextureType_UNKNOWN) > 0)
				{
					for (uint32_t textureIndex = 0; textureIndex < material->GetTextureCount(aiTextureType_UNKNOWN); textureIndex++)
					{
						aiString textureName;
						material->Get(AI_MATKEY_TEXTURE(aiTextureType_UNKNOWN, textureIndex), textureName);

						LOG_CORE_TRACE(" UNKNOWN filename: {}", textureName.C_Str());
					}
				}

				if (material->GetTextureCount(aiTextureType_REFLECTION) > 0)
				{
					LOG_CORE_TRACE(" Found REFLECTION texture: {}", material->GetTextureCount(aiTextureType_REFLECTION));
				}

				if (material->GetTextureCount(aiTextureType_METALNESS) > 0)
				{
					for (uint32_t textureIndex = 0; textureIndex < material->GetTextureCount(aiTextureType_METALNESS); textureIndex++)
					{
						aiString textureName;
						material->Get(AI_MATKEY_TEXTURE(aiTextureType_METALNESS, textureIndex), textureName);

						LOG_CORE_TRACE(" METALNESS filename: {}", textureName.C_Str());

						newMaterial.HasMetalnessMap = true;
						newMaterial.MetalnessMap.TextureName = "assets/textures/" + sceneName + "/" + ConvertFilePath(textureName.C_Str());
					}
				}

				if (material->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS) > 0)
				{
					for (uint32_t textureIndex = 0; textureIndex < material->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS); textureIndex++)
					{
						aiString textureName;
						material->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE_ROUGHNESS, textureIndex), textureName);

						LOG_CORE_TRACE(" DIFFUSE ROUGHNESS filename: {}", textureName.C_Str());

						newMaterial.HasRoughnessMap = true;
						newMaterial.RoughnessMap.TextureName = "assets/textures/" + sceneName + "/" + ConvertFilePath(textureName.C_Str());
					}
				}

				if (material->GetTextureCount(aiTextureType_NORMALS) > 0)
				{
					for (uint32_t normalsIndex = 0; normalsIndex < material->GetTextureCount(aiTextureType_NORMALS); normalsIndex++)
					{
						aiString textureName;
						material->Get(AI_MATKEY_TEXTURE_NORMALS(normalsIndex), textureName);

						LOG_CORE_TRACE(" NORMALS filename: {}", textureName.C_Str());

						newMaterial.HasNormalMap = true;
						newMaterial.NormalMap.TextureName = "assets/textures/" + sceneName + "/" + ConvertFilePath(textureName.C_Str());
					}
				}

				if (material->GetTextureCount(aiTextureType_HEIGHT) > 0)
				{
					for (uint32_t heightIndex = 0; heightIndex < material->GetTextureCount(aiTextureType_HEIGHT); heightIndex++)
					{
						aiString textureName;
						material->Get(AI_MATKEY_TEXTURE_HEIGHT(heightIndex), textureName);

						LOG_CORE_TRACE(" HEIGHT filename: {}", textureName.C_Str());

						newMaterial.HasNormalMap = true;
						newMaterial.NormalMap.TextureName = "assets/textures/" + sceneName + "/" + ConvertFilePath(textureName.C_Str());
					}
				}

				if (material->GetTextureCount(aiTextureType_SHININESS) > 0)
				{
					for (uint32_t shininessIndex = 0; shininessIndex < material->GetTextureCount(aiTextureType_SHININESS); shininessIndex++)
					{
						aiString textureName;
						material->Get(AI_MATKEY_TEXTURE_SHININESS(shininessIndex), textureName);

						LOG_CORE_TRACE(" SHININESS filename: {}", textureName.C_Str());

						newMaterial.HasRoughnessMap = true;
						newMaterial.RoughnessMap.TextureName = "assets/textures/" + sceneName + "/" + ConvertFilePath(textureName.C_Str());
					}
				}

				aiColor3D diffuseColor;
				material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);

				newMaterial.Diffuse.r = diffuseColor.r;
				newMaterial.Diffuse.g = diffuseColor.g;
				newMaterial.Diffuse.b = diffuseColor.b;

				aiColor3D transparency;
				material->Get(AI_MATKEY_COLOR_TRANSPARENT, transparency);

				newMaterial.Transparency = transparency.r;


				importResult.Materials.emplace_back(newMaterial);
			}
		}

		if (scene->HasTextures())
		{
			for (uint32_t i = 0; i < scene->mNumTextures; i++)
			{
				auto* texture = scene->mTextures[i];
				LOG_CORE_TRACE("Texture: {}", texture->mFilename.C_Str());
				
				std::string filename = "assets/textures/" + sceneName + "/" + ConvertFilePath(texture->mFilename.C_Str());

				Ref<Vulkan::Texture2D> newTexture = Vulkan::Texture2D::Create(filename, Vulkan::TextureFormat::R8G8B8A8_Normalized, texture->mWidth, texture->mHeight, (const char*)texture->pcData, sizeof(aiTexel));
//				Ref<Vulkan::Texture2D> newTexture = Vulkan::Texture2D::Create(texture->mFilename.C_Str(), Vulkan::TextureFormat::R8G8B8A8_Normalized, texture->mWidth, texture->mHeight, (const char*)texture->pcData, sizeof(aiTexel));

				ResourceManager::AddTexture(newTexture);
			}
		}


//		aiReleaseImport(scene);
		importer.FreeScene();

		return importResult;
	}


	void ModelImport::GetNodeData(const aiScene* scene, aiNode* node, ImportResult& importResult, glm::mat4 transform)
	{
		for (uint32_t i = 0; i < node->mNumMeshes; i++)
		{
			auto* mesh = scene->mMeshes[node->mMeshes[i]];

//			transform = aiMatrix4x4ToGlmMat4(node->mTransformation) * transform;
//			transform = transform * aiMatrix4x4ToGlmMat4(node->mTransformation);

			ImportMesh newMesh;
			newMesh.ObjectName = mesh->mName.C_Str();
			newMesh.MaterialName = scene->mMaterials[mesh->mMaterialIndex]->GetName().C_Str();

//			newMesh.Transform = aiMatrix4x4ToGlmMat4(node->mTransformation) * transform;
			newMesh.Transform = transform * aiMatrix4x4ToGlmMat4(node->mTransformation);

			if (newMesh.MaterialName == "")
			{
				newMesh.MaterialName = "Material_" + std::to_string(mesh->mMaterialIndex);
			}

			LOG_CORE_TRACE("Mesh found: {} (material: {})", newMesh.ObjectName, newMesh.MaterialName);

			for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; vertexIndex++)
			{
				Vertex newVertex;

				if (mesh->HasPositions())
				{
					newVertex.Position.x = mesh->mVertices[vertexIndex].x;
					newVertex.Position.y = mesh->mVertices[vertexIndex].y;
					newVertex.Position.z = mesh->mVertices[vertexIndex].z;
				}

				if (mesh->HasNormals())
				{
					newVertex.Normal.x = mesh->mNormals[vertexIndex].x;
					newVertex.Normal.y = mesh->mNormals[vertexIndex].y;
					newVertex.Normal.z = mesh->mNormals[vertexIndex].z;
				}

				if (mesh->HasTextureCoords(0))
				{
					newVertex.TexCoord.x = mesh->mTextureCoords[0][vertexIndex].x;
					newVertex.TexCoord.y = mesh->mTextureCoords[0][vertexIndex].y;
				}

				if (mesh->HasVertexColors(vertexIndex))
				{
					newVertex.Color.r = mesh->mColors[vertexIndex]->r;
					newVertex.Color.g = mesh->mColors[vertexIndex]->g;
					newVertex.Color.b = mesh->mColors[vertexIndex]->b;
					newVertex.Color.a = mesh->mColors[vertexIndex]->a;
				}

				if (mesh->HasTangentsAndBitangents())
				{
					newVertex.Tangent.x = mesh->mTangents[vertexIndex].x;
					newVertex.Tangent.y = mesh->mTangents[vertexIndex].y;
					newVertex.Tangent.z = mesh->mTangents[vertexIndex].z;

					newVertex.Bitangent.x = mesh->mBitangents[vertexIndex].x;
					newVertex.Bitangent.y = mesh->mBitangents[vertexIndex].y;
					newVertex.Bitangent.z = mesh->mBitangents[vertexIndex].z;
				}

				newMesh.VertexList.emplace_back(newVertex);
			}

			for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex++)
			{
				aiFace face = mesh->mFaces[faceIndex];

				for (uint32_t j = 0; j < face.mNumIndices; j++)
				{
					newMesh.IndexList.emplace_back(face.mIndices[j]);
				}

			}

			if (!mesh->HasTangentsAndBitangents())
			{
				for (uint32_t triangleIndex = 0; triangleIndex < (uint32_t)newMesh.IndexList.size(); triangleIndex += 3)
				{
					CalculateTangentAndBitangent(newMesh.VertexList, newMesh.IndexList[triangleIndex], newMesh.IndexList[triangleIndex + 1], newMesh.IndexList[triangleIndex + 2]);
				}
			}


			LOG_CORE_WARN("Imported: {} vertices, {} indices", (uint32_t)newMesh.VertexList.size(), (uint32_t)newMesh.IndexList.size());

			importResult.Meshes.emplace_back(newMesh);

		}

		for (uint32_t i = 0; i < node->mNumChildren; i++)
		{
			GetNodeData(scene, node->mChildren[i], importResult, transform * aiMatrix4x4ToGlmMat4(node->mTransformation));
		}
		
	}


	void ModelImport::CalculateTangentAndBitangent(std::vector<Vertex>& vertexList, uint32_t index0, uint32_t index1, uint32_t index2)
	{
		Vertex& v1 = vertexList[index0];
		Vertex& v2 = vertexList[index1];
		Vertex& v3 = vertexList[index2];

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
