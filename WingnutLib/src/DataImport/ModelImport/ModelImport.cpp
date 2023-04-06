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
		ImportResult importResult;

		// Extract model name from filepath (strip extension)
		auto startLocation = filepath.find_last_of('/');
		auto endLocation = filepath.find('.');

		if (startLocation == std::string::npos)
		{
			startLocation = 0;
		}

		if (endLocation == std::string::npos)
		{
			importResult.ModelName = filepath.substr(startLocation + 1);
		}
		else
		{
			importResult.ModelName = filepath.substr(startLocation + 1, (endLocation - (startLocation + 1)));
		}

		auto importer = Assimp::Importer();
		importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_READ_ALL_MATERIALS, false);
		const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_CalcTangentSpace | aiProcess_ValidateDataStructure); // | aiProcess_FlipUVs);

		if (!scene || scene->mRootNode == nullptr)
		{
			LOG_CORE_ERROR("[ModelImport] Failed to load model {}", filepath);
			LOG_CORE_ERROR("[ModelImport] {}", aiGetErrorString());

			return ImportResult();
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

					if (std::string(property->mKey.C_Str()) == "$clr.ambient")
					{
						aiColor3D color;
						material->Get(AI_MATKEY_COLOR_AMBIENT, color);

						newMaterial.Ambient.r = color.r;
						newMaterial.Ambient.g = color.g;
						newMaterial.Ambient.b = color.b;

						LOG_CORE_TRACE(" {} : {},{},{}", property->mKey.C_Str(), color.r, color.g, color.b);
					}

					if (std::string(property->mKey.C_Str()) == "$clr.diffuse")
					{
						aiColor3D color;
						material->Get(AI_MATKEY_COLOR_DIFFUSE, color);

						newMaterial.Diffuse.r = color.r;
						newMaterial.Diffuse.g = color.g;
						newMaterial.Diffuse.b = color.b;

						LOG_CORE_TRACE(" {} : {},{},{}", property->mKey.C_Str(), color.r, color.g, color.b);
					}

					if (std::string(property->mKey.C_Str()) == "$clr.base")
					{
						aiColor3D color;
						material->Get(AI_MATKEY_BASE_COLOR, color);

						LOG_CORE_TRACE(" {} : {},{},{}", property->mKey.C_Str(), color.r, color.g, color.b);
					}

					if (std::string(property->mKey.C_Str()) == "$clr.specular")
					{
						aiColor3D color;
						material->Get(AI_MATKEY_COLOR_SPECULAR, color);

						LOG_CORE_TRACE(" {} : {},{},{}", property->mKey.C_Str(), color.r, color.g, color.b);
					}

					if (std::string(property->mKey.C_Str()) == "$clr.reflective")
					{
						aiColor3D color;
						material->Get(AI_MATKEY_COLOR_REFLECTIVE, color);

						LOG_CORE_TRACE(" {} : {},{},{}", property->mKey.C_Str(), color.r, color.g, color.b);
					}

					if (std::string(property->mKey.C_Str()) == "$clr.emissive")
					{
						aiColor3D color;
						material->Get(AI_MATKEY_COLOR_EMISSIVE, color);

						LOG_CORE_TRACE(" {} : {},{},{}", property->mKey.C_Str(), color.r, color.g, color.b);
					}

					if (std::string(property->mKey.C_Str()) == "$clr.transparent")
					{
						aiColor3D transparency;
						material->Get(AI_MATKEY_COLOR_TRANSPARENT, transparency);

						newMaterial.Transparency = transparency.r;

						LOG_CORE_TRACE(" {} : {},{},{}", property->mKey.C_Str(), transparency.r, transparency.g, transparency.b);
					}

//					if (std::string(property->mKey.C_Str()) == "$mat.metallicFactor")
					if (std::string(property->mKey.C_Str()) == "$mat.reflectivity")
					{
						float factor;
//						material->Get(AI_MATKEY_METALLIC_FACTOR, factor);
						material->Get(AI_MATKEY_REFLECTIVITY, factor);

						newMaterial.Metalness = factor;
						newMaterial.HasPBRValues = true;

						LOG_CORE_TRACE(" {} : {}", property->mKey.C_Str(), factor);
					}

					if (std::string(property->mKey.C_Str()) == "$mat.roughnessFactor")
					{
						float factor;
						material->Get(AI_MATKEY_ROUGHNESS_FACTOR, factor);

						newMaterial.Roughness = factor;
						newMaterial.HasPBRValues = true;

						LOG_CORE_TRACE(" {} : {}", property->mKey.C_Str(), factor);
					}

					if (std::string(property->mKey.C_Str()) == "$mat.transparencyfactor")
					{
						float factor;
						material->Get(AI_MATKEY_TRANSPARENCYFACTOR, factor);

						LOG_CORE_TRACE(" {} : {}", property->mKey.C_Str(), factor);
					}

					if (std::string(property->mKey.C_Str()) == "$mat.bumpscaling")
					{
						float factor;
						material->Get(AI_MATKEY_BUMPSCALING, factor);

						LOG_CORE_TRACE(" {} : {}", property->mKey.C_Str(), factor);
					}

/*					if (std::string(property->mKey.C_Str()) == "$mat.shininess")
					{
						float value;
						material->Get(AI_MATKEY_SHININESS, value);

						newMaterial.Metalness = value;

						LOG_CORE_TRACE(" {} : {}", property->mKey.C_Str(), value);
					}
*/
					if (std::string(property->mKey.C_Str()) == "$mat.opacity")
					{
						float value;
						material->Get(AI_MATKEY_OPACITY, value);

						LOG_CORE_TRACE(" {} : {}", property->mKey.C_Str(), value);
					}
				}

				if (material->GetTextureCount(aiTextureType_BASE_COLOR) > 0)
				{
					for (uint32_t baseColorIndex = 0; baseColorIndex < material->GetTextureCount(aiTextureType_DIFFUSE); baseColorIndex++)
					{
						aiString textureName;
						material->Get(AI_MATKEY_TEXTURE(aiTextureType_BASE_COLOR, baseColorIndex), textureName);

						std::string texName = textureName.C_Str();

						if (texName.find('*') != std::string::npos)
						{
							const aiTexture* embeddedTexture = scene->GetEmbeddedTexture(texName.c_str());
							newMaterial.DiffuseTexture.TextureName = "assets/" + std::string(material->GetName().C_Str()) + "/textures/" + GetFilenameFromPath(embeddedTexture->mFilename.C_Str());
						}
						else
						{
							newMaterial.DiffuseTexture.TextureName = "assets/" + std::string(material->GetName().C_Str()) + "/textures/" + GetFilenameFromPath(textureName.C_Str());
						}

						LOG_CORE_TRACE(" BASE COLOR filename: {}", newMaterial.DiffuseTexture.TextureName);

						newMaterial.HasDiffuseTexture = true;

						LoadTexture(scene, newMaterial.DiffuseTexture.TextureName);
					}

				}

				if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
				{
					for (uint32_t diffuseIndex = 0; diffuseIndex < material->GetTextureCount(aiTextureType_DIFFUSE); diffuseIndex++)
					{
						aiString textureName;
						material->Get(AI_MATKEY_TEXTURE_DIFFUSE(diffuseIndex), textureName);

						std::string texName = textureName.C_Str();

						if (texName.find('*') != std::string::npos)
						{
							const aiTexture* embeddedTexture = scene->GetEmbeddedTexture(texName.c_str());
							newMaterial.DiffuseTexture.TextureName = "assets/" + std::string(material->GetName().C_Str()) + "/textures/" + GetFilenameFromPath(embeddedTexture->mFilename.C_Str());
						}
						else
						{
							newMaterial.DiffuseTexture.TextureName = "assets/" + std::string(material->GetName().C_Str()) + "/textures/" + GetFilenameFromPath(textureName.C_Str());
						}

						LOG_CORE_TRACE(" DIFFUSE filename: {}", newMaterial.DiffuseTexture.TextureName);

						newMaterial.HasDiffuseTexture = true;

						LoadTexture(scene, newMaterial.DiffuseTexture.TextureName);
					}

				}

				if (material->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION) > 0)
				{
					for (uint32_t textureIndex = 0; textureIndex < material->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION); textureIndex++)
					{
						aiString textureName;
						material->Get(AI_MATKEY_TEXTURE(aiTextureType_AMBIENT_OCCLUSION, textureIndex), textureName);

						std::string texName = textureName.C_Str();

						if (texName.find('*') != std::string::npos)
						{
							const aiTexture* embeddedTexture = scene->GetEmbeddedTexture(texName.c_str());
							newMaterial.AmbientOcclusionMap.TextureName = "assets/" + std::string(material->GetName().C_Str()) + "/textures/" + GetFilenameFromPath(embeddedTexture->mFilename.C_Str());
						}
						else
						{
							newMaterial.AmbientOcclusionMap.TextureName = "assets/" + std::string(material->GetName().C_Str()) + "/textures/" + GetFilenameFromPath(textureName.C_Str());
						}

						LOG_CORE_TRACE(" AMBIENT OCCLUSION filename: {}", newMaterial.AmbientOcclusionMap.TextureName);

						newMaterial.HasAmbientOcclusionMap = true;

						LoadTexture(scene, newMaterial.AmbientOcclusionMap.TextureName);
					}
				}

				if (material->GetTextureCount(aiTextureType_SPECULAR) > 0)
				{
					for (uint32_t specularIndex = 0; specularIndex < material->GetTextureCount(aiTextureType_DIFFUSE); specularIndex++)
					{
						aiString textureName;
						material->Get(AI_MATKEY_TEXTURE_SPECULAR(specularIndex), textureName);

						std::string texName = textureName.C_Str();

						if (texName.find('*') != std::string::npos)
						{
							const aiTexture* embeddedTexture = scene->GetEmbeddedTexture(texName.c_str());
							newMaterial.MetalnessMap.TextureName = "assets/" + std::string(material->GetName().C_Str()) + "/textures/" + GetFilenameFromPath(embeddedTexture->mFilename.C_Str());
						}
						else
						{
							newMaterial.MetalnessMap.TextureName = "assets/" + std::string(material->GetName().C_Str()) + "/textures/" + GetFilenameFromPath(textureName.C_Str());
						}

						LOG_CORE_TRACE(" SPECULAR filename: {}", newMaterial.MetalnessMap.TextureName);

						newMaterial.HasMetalnessMap = true;

						LoadTexture(scene, newMaterial.MetalnessMap.TextureName);
					}
				}

				if (material->GetTextureCount(aiTextureType_UNKNOWN) > 0)
				{
					for (uint32_t textureIndex = 0; textureIndex < material->GetTextureCount(aiTextureType_UNKNOWN); textureIndex++)
					{
						aiString textureName;
						material->Get(AI_MATKEY_TEXTURE(aiTextureType_UNKNOWN, textureIndex), textureName);

						std::string texName = textureName.C_Str();

						if (texName.find('*') != std::string::npos)
						{
							const aiTexture* embeddedTexture = scene->GetEmbeddedTexture(texName.c_str());
//							newMaterial.DiffuseTexture.TextureName = "assets/textures/" + importResult.ModelName + "/" + embeddedTexture->mFilename.C_Str();
						}
						else
						{
//							newMaterial.DiffuseTexture.TextureName = "assets/textures/" + importResult.ModelName + "/" + ConvertFilePath(textureName.C_Str());
						}

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

						std::string texName = textureName.C_Str();

						if (texName.find('*') != std::string::npos)
						{
							const aiTexture* embeddedTexture = scene->GetEmbeddedTexture(texName.c_str());
							newMaterial.MetalnessMap.TextureName = "assets/" + std::string(material->GetName().C_Str()) + "/textures/" + GetFilenameFromPath(embeddedTexture->mFilename.C_Str());
						}
						else
						{
							newMaterial.MetalnessMap.TextureName = "assets/" + std::string(material->GetName().C_Str()) + "/textures/" + GetFilenameFromPath(textureName.C_Str());
						}

						LOG_CORE_TRACE(" METALNESS filename: {}", newMaterial.MetalnessMap.TextureName);

						newMaterial.HasMetalnessMap = true;

						LoadTexture(scene, newMaterial.MetalnessMap.TextureName);
					}
				}

				if (material->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS) > 0)
				{
					for (uint32_t textureIndex = 0; textureIndex < material->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS); textureIndex++)
					{
						aiString textureName;
						material->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE_ROUGHNESS, textureIndex), textureName);

						std::string texName = textureName.C_Str();

						if (texName.find('*') != std::string::npos)
						{
							const aiTexture* embeddedTexture = scene->GetEmbeddedTexture(texName.c_str());
							newMaterial.RoughnessMap.TextureName = "assets/" + std::string(material->GetName().C_Str()) + "/textures/" + GetFilenameFromPath(embeddedTexture->mFilename.C_Str());
						}
						else
						{
							newMaterial.RoughnessMap.TextureName = "assets/" + std::string(material->GetName().C_Str()) + "/textures/" + GetFilenameFromPath(textureName.C_Str());
						}

						LOG_CORE_TRACE(" DIFFUSE ROUGHNESS filename: {}", newMaterial.RoughnessMap.TextureName);

						newMaterial.HasRoughnessMap = true;

						LoadTexture(scene, newMaterial.RoughnessMap.TextureName);
					}
				}

				if (material->GetTextureCount(aiTextureType_NORMALS) > 0)
				{
					for (uint32_t normalsIndex = 0; normalsIndex < material->GetTextureCount(aiTextureType_NORMALS); normalsIndex++)
					{
						aiString textureName;
						material->Get(AI_MATKEY_TEXTURE_NORMALS(normalsIndex), textureName);

						std::string texName = textureName.C_Str();

						if (texName.find('*') != std::string::npos)
						{
							const aiTexture* embeddedTexture = scene->GetEmbeddedTexture(texName.c_str());
							newMaterial.NormalMap.TextureName = "assets/" + std::string(material->GetName().C_Str()) + "/textures/" + GetFilenameFromPath(embeddedTexture->mFilename.C_Str());
						}
						else
						{
							newMaterial.NormalMap.TextureName = "assets/" + std::string(material->GetName().C_Str()) + "/textures/" + GetFilenameFromPath(textureName.C_Str());
						}

						LOG_CORE_TRACE(" NORMALS filename: {}", newMaterial.NormalMap.TextureName);

						newMaterial.HasNormalMap = true;

						LoadTexture(scene, newMaterial.NormalMap.TextureName);
					}
				}

				if (material->GetTextureCount(aiTextureType_HEIGHT) > 0)
				{
					for (uint32_t heightIndex = 0; heightIndex < material->GetTextureCount(aiTextureType_HEIGHT); heightIndex++)
					{
						aiString textureName;
						material->Get(AI_MATKEY_TEXTURE_HEIGHT(heightIndex), textureName);

						std::string texName = textureName.C_Str();

						if (texName.find('*') != std::string::npos)
						{
							const aiTexture* embeddedTexture = scene->GetEmbeddedTexture(texName.c_str());
							newMaterial.NormalMap.TextureName = "assets/" + std::string(material->GetName().C_Str()) + "/textures/" + GetFilenameFromPath(embeddedTexture->mFilename.C_Str());
						}
						else
						{
							newMaterial.NormalMap.TextureName = "assets/" + std::string(material->GetName().C_Str()) + "/textures/" + GetFilenameFromPath(textureName.C_Str());
						}

						LOG_CORE_TRACE(" HEIGHT filename: {}", newMaterial.NormalMap.TextureName);

						newMaterial.HasNormalMap = true;

						LoadTexture(scene, newMaterial.NormalMap.TextureName);
					}
				}

				if (material->GetTextureCount(aiTextureType_SHININESS) > 0)
				{
					for (uint32_t shininessIndex = 0; shininessIndex < material->GetTextureCount(aiTextureType_SHININESS); shininessIndex++)
					{
						aiString textureName;
						material->Get(AI_MATKEY_TEXTURE_SHININESS(shininessIndex), textureName);

						std::string texName = textureName.C_Str();

						if (texName.find('*') != std::string::npos)
						{
							const aiTexture* embeddedTexture = scene->GetEmbeddedTexture(texName.c_str());
							newMaterial.RoughnessMap.TextureName = "assets/" + std::string(material->GetName().C_Str()) + "/textures/" + GetFilenameFromPath(embeddedTexture->mFilename.C_Str());
						}
						else
						{
							newMaterial.RoughnessMap.TextureName = "assets/" + std::string(material->GetName().C_Str()) + "/textures/" + GetFilenameFromPath(textureName.C_Str());
						}

						LOG_CORE_TRACE(" SHININESS filename: {}", newMaterial.RoughnessMap.TextureName);

						newMaterial.HasRoughnessMap = true;

						LoadTexture(scene, newMaterial.RoughnessMap.TextureName);
					}
				}

				if (material->GetTextureCount(aiTextureType_EMISSIVE) > 0)
				{
					for (uint32_t index = 0; index < material->GetTextureCount(aiTextureType_EMISSIVE); index++)
					{
						aiString textureName;
						material->Get(AI_MATKEY_TEXTURE_EMISSIVE(index), textureName);

						std::string texName = textureName.C_Str();

						if (texName.find('*') != std::string::npos)
						{
							const aiTexture* embeddedTexture = scene->GetEmbeddedTexture(texName.c_str());
//							newMaterial.RoughnessMap.TextureName = "assets/textures/" + importResult.ModelName + "/" + embeddedTexture->mFilename.C_Str();
							LOG_CORE_TRACE(" EMISSIVE filename: {}", embeddedTexture->mFilename.C_Str());
						}
						else
						{
							LOG_CORE_TRACE(" EMISSIVE filename: {}", textureName.C_Str());

//							newMaterial.RoughnessMap.TextureName = "assets/textures/" + importResult.ModelName + "/" + ConvertFilePath(textureName.C_Str());
						}

//						newMaterial.HasRoughnessMap = true;

//						LoadTexture(scene, newMaterial.DiffuseTexture.TextureName, AssetTextureType::Emissive);
					}
				}


				importResult.Materials.emplace_back(newMaterial);
			}
		}

		importer.FreeScene();

		return importResult;
	}


	void ModelImport::LoadTexture(const aiScene* scene, const std::string& textureName)
	{
		aiTexture* texture = nullptr;

		for (uint32_t i = 0; i < scene->mNumTextures; i++)
		{
			if (GetFilenameFromPath(scene->mTextures[i]->mFilename.C_Str()) == GetFilenameFromPath(textureName))
			{
				texture = scene->mTextures[i];
				break;
			}
		}

		if (texture == nullptr)
		{
			LOG_CORE_WARN("[ModelImport] Texture {} not embedded in file", textureName);
			return;
		}

		Ref<Vulkan::Texture2D> newTexture = Vulkan::Texture2D::Create(textureName, Vulkan::TextureFormat::R8G8B8A8_Normalized, texture->mWidth, texture->mHeight, (const char*)texture->pcData, sizeof(aiTexel));

		ResourceManager::AddTexture(newTexture);
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
