#include "wingnut_pch.h"
#include "ModelImport.h"

#include "assimp/cimport.h"
#include "assimp/Importer.hpp"
#include "assimp/material.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Assets/ResourceManager.h"

#include "Platform/Vulkan/Texture.h"


namespace Wingnut
{


	void ModelImport::ImportFBX(const std::string& filepath)
	{

		auto importer = Assimp::Importer();
		importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_READ_ALL_MATERIALS, false);
		const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_CalcTangentSpace | aiProcess_ValidateDataStructure);

		if (!scene)
		{
			LOG_CORE_ERROR("[ModelImport] Failed to load model {}", filepath);

			LOG_CORE_ERROR("[ModelImport] {}", aiGetErrorString());

			return;
		}

		if (scene->HasMaterials())
		{
			for (uint32_t i = 0; i < scene->mNumMaterials; i++)
			{
				auto* material = scene->mMaterials[i];

				LOG_CORE_TRACE("Material found: {}", material->GetName().C_Str());

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
					}

				}

				if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
				{
					for (uint32_t diffuseIndex = 0; diffuseIndex < material->GetTextureCount(aiTextureType_DIFFUSE); diffuseIndex++)
					{
						aiString textureName;
						material->Get(AI_MATKEY_TEXTURE_DIFFUSE(diffuseIndex), textureName);

						LOG_CORE_TRACE(" DIFFUSE filename: {}", textureName.C_Str());
					}

				}

				if (material->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION) > 0)
				{
					for (uint32_t textureIndex = 0; textureIndex < material->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION); textureIndex++)
					{
						aiString textureName;
						material->Get(AI_MATKEY_TEXTURE(aiTextureType_AMBIENT_OCCLUSION, textureIndex), textureName);

						LOG_CORE_TRACE(" AMBIENT OCCLUSION filename: {}", textureName.C_Str());
					}
				}

				if (material->GetTextureCount(aiTextureType_SPECULAR) > 0)
				{
					for (uint32_t specularIndex = 0; specularIndex < material->GetTextureCount(aiTextureType_DIFFUSE); specularIndex++)
					{
						aiString textureName;
						material->Get(AI_MATKEY_TEXTURE_SPECULAR(specularIndex), textureName);

						LOG_CORE_TRACE(" SPECULAR filename: {}", textureName.C_Str());
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
					}
				}

				if (material->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS) > 0)
				{
					for (uint32_t textureIndex = 0; textureIndex < material->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS); textureIndex++)
					{
						aiString textureName;
						material->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE_ROUGHNESS, textureIndex), textureName);

						LOG_CORE_TRACE(" DIFFUSE ROUGHNESS filename: {}", textureName.C_Str());
					}
				}

				if (material->GetTextureCount(aiTextureType_NORMALS) > 0)
				{
					for (uint32_t normalsIndex = 0; normalsIndex < material->GetTextureCount(aiTextureType_NORMALS); normalsIndex++)
					{
						aiString textureName;
						material->Get(AI_MATKEY_TEXTURE_NORMALS(normalsIndex), textureName);

						LOG_CORE_TRACE(" NORMALS filename: {}", textureName.C_Str());
					}
				}

				if (material->GetTextureCount(aiTextureType_HEIGHT) > 0)
				{
					for (uint32_t heightIndex = 0; heightIndex < material->GetTextureCount(aiTextureType_HEIGHT); heightIndex++)
					{
						aiString textureName;
						material->Get(AI_MATKEY_TEXTURE_HEIGHT(heightIndex), textureName);

						LOG_CORE_TRACE(" HEIGHT filename: {}", textureName.C_Str());
					}
				}

				if (material->GetTextureCount(aiTextureType_SHININESS) > 0)
				{
					for (uint32_t shininessIndex = 0; shininessIndex < material->GetTextureCount(aiTextureType_SHININESS); shininessIndex++)
					{
						aiString textureName;
						material->Get(AI_MATKEY_TEXTURE_SHININESS(shininessIndex), textureName);

						LOG_CORE_TRACE(" SHININESS filename: {}", textureName.C_Str());
					}
				}

			}
		}

		if (scene->HasTextures())
		{
			for (uint32_t i = 0; i < scene->mNumTextures; i++)
			{
				auto* texture = scene->mTextures[i];
				LOG_CORE_TRACE("Texture: {}", texture->mFilename.C_Str());

				Ref<Vulkan::Texture2D> newTexture = Vulkan::Texture2D::Create(texture->mFilename.C_Str(), Vulkan::TextureFormat::R8G8B8A8_Normalized, texture->mWidth, texture->mHeight, (const char*)texture->pcData, sizeof(aiTexel));

				ResourceManager::AddTexture(newTexture);
			}
		}


//		aiReleaseImport(scene);
		importer.FreeScene();
	}



}
