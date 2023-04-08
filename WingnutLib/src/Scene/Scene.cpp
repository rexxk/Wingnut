#include "wingnut_pch.h"
#include "Scene.h"

#include "Assets/ResourceManager.h"

#include "Core/Timer.h"

#include "DataImport/ModelImport/ModelImport.h"
#include "DataImport/Obj/ObjLoader.h"

#include "Event/EventUtils.h"
#include "Event/WindowEvents.h"

#include "File/Serializer.h"
#include "File/VirtualFileSystem.h"

#include "Components.h"
#include "Entity.h"

#include "Platform/Vulkan/Texture.h"

#include "Renderer/Material.h"
#include "Renderer/Renderer.h"


namespace Wingnut
{


	Ref<Scene> Scene::Create(const SceneProperties& sceneProperties)
	{
		return CreateRef<Scene>(sceneProperties);
	}


	Scene::Scene(const SceneProperties& properties)
	{
		auto& rendererData = Renderer::GetContext()->GetRendererData();

		m_SceneExtent = properties.SceneExtent;
		m_SceneCamera = properties.SceneCamera;

		m_SceneRenderer = SceneRenderer::Create(m_SceneExtent);

		m_CameraUB = Vulkan::UniformBuffer::Create(rendererData.Device, sizeof(CameraData));
		m_CameraDescriptor = Vulkan::Descriptor::Create(rendererData.Device, ResourceManager::GetShader(ShaderType::Default), CameraDescriptor);
		m_CameraDescriptor->SetBufferBinding(0, m_CameraUB);
		m_CameraDescriptor->UpdateBindings();

		m_LightUB = Vulkan::UniformBuffer::Create(rendererData.Device, sizeof(LightData));
		m_LightDescriptor = Vulkan::Descriptor::Create(rendererData.Device, ResourceManager::GetShader(ShaderType::Default), LightsDescriptor);
		m_LightDescriptor->SetBufferBinding(0, m_LightUB);
		m_LightDescriptor->UpdateBindings();

		m_EntityRegistry = CreateRef<ECS::Registry>();

		ClearScene(true);

	}

	Scene::~Scene()
	{

	}

	void Scene::Release()
	{

		if (m_LightUB)
		{
			m_LightUB->Release();
		}

		if (m_CameraUB)
		{
			m_CameraUB->Release();
		}

		if (m_SceneRenderer)
		{
			m_SceneRenderer->Release();
		}

	}

	void Scene::ClearScene(bool withLight)
	{
		for (auto& entity : m_SceneEntities)
		{
			if (entity.HasComponent<TagComponent>())
			{
				entity.RemoveComponent<TagComponent>();
			}

			if (entity.HasComponent<TransformComponent>())
			{
				entity.RemoveComponent<TransformComponent>();
			}

			if (entity.HasComponent<MeshComponent>())
			{
				entity.RemoveComponent<MeshComponent>();
			}

			if (entity.HasComponent<MaterialComponent>())
			{
				entity.RemoveComponent<MaterialComponent>();
			}

			if (entity.HasComponent<LightComponent>())
			{
				entity.RemoveComponent<LightComponent>();
			}
		}

		m_SceneEntities.clear();

		m_SceneRenderer->Clear();

		ResourceManager::ClearMaterials();
		ResourceManager::ClearTextures();

		if (withLight)
		{
			Entity light = CreateEntity("Light");
			light.AddComponent<LightComponent>(glm::vec3(0.0f, 15.0f, 0.0f), glm::vec3(210.0f, 235.0f, 205.0f));
		}

		m_SceneCamera->SetPosition(glm::vec3(0.0f, 0.0f, -8.0f));
		m_SceneCamera->SetLookAt(glm::vec3(0.0f));
		m_SceneCamera->SetRotation(glm::vec3(0.0f));

	}

	void Scene::Begin()
	{
		auto& rendererData = Renderer::GetContext()->GetRendererData();
		uint32_t currentFrame = Renderer::GetContext()->GetCurrentFrame();

		m_SceneRenderer->BeginScene(currentFrame);

		CameraData cameraData = {};
		cameraData.ViewProjection = m_SceneCamera->GetViewProjectionMatrix();

		m_CameraUB->Update(&cameraData, sizeof(CameraData), currentFrame);

		LightData lightData;

		for (auto& lightEntity : ECS::EntitySystem::GetView<LightComponent>())
		{
			LightComponent& lightComponent = ECS::EntitySystem::GetComponent<LightComponent>(lightEntity);
			lightData.LightPosition = lightComponent.Position;
			lightData.LightColor = lightComponent.Color;

			lightData.CameraPosition = m_SceneCamera->GetPosition();
		}

		m_LightUB->Update(&lightData, sizeof(LightData), currentFrame);

		m_SceneRenderer->SubmitDescriptor(m_CameraDescriptor);
		m_SceneRenderer->SubmitDescriptor(m_LightDescriptor);

	}

	void Scene::End()
	{

		m_SceneRenderer->EndScene();

	}


	void Scene::Draw()
	{
		auto& device = Renderer::GetContext()->GetRendererData().Device;

		auto& entities = ECS::EntitySystem::GetView<MeshComponent>();

		Timer drawSceneMetrics;

		for (auto entity : entities)
		{
			auto& meshComponent = ECS::EntitySystem::GetComponent<MeshComponent>(entity);

			glm::mat4 transform = glm::mat4(1.0f);

			if (ECS::EntitySystem::HasComponent<TransformComponent>(entity))
			{
				transform = ECS::EntitySystem::GetComponent<TransformComponent>(entity).Transform;
			}

			Ref<Material> material = nullptr;

			if (ECS::EntitySystem::HasComponent<MaterialComponent>(entity))
			{
				material = ResourceManager::GetMaterial(ECS::EntitySystem::GetComponent<MaterialComponent>(entity).MaterialID);
			}

			m_SceneRenderer->SubmitToDrawList(entity, meshComponent.VertexList, meshComponent.IndexList, transform, material);
		}

		Application::Get().GetMetrics().SceneDrawTime = (float)drawSceneMetrics.ElapsedTime();

		Timer renderTimeMetrics;

		m_SceneRenderer->Draw();

		Application::Get().GetMetrics().RenderingTime = (float)renderTimeMetrics.ElapsedTime();

	}


	Entity Scene::CreateEntity(const std::string& tag, UUID entityID)
	{
		Entity entity(ECS::EntitySystem::Create(m_EntityRegistry));

		if (entityID != 0)
		{
			entity.SetID(entityID);
		}

		entity.AddComponent<TagComponent>(tag);

		m_SceneEntities.emplace_back(entity);

		return entity;
	}

	void Scene::Update(Timestep ts)
	{
		m_SceneCamera->Update(ts);
	}

	void Scene::ImportModel(const std::string& filepath)
	{
		ImportResult importResult = ModelImport::ImportFBX(filepath);

		for (auto& material : importResult.Materials)
		{
			Ref<Material> newMaterial = PBRMaterial::Create(material, ResourceManager::GetShader(ShaderType::Default), ResourceManager::GetSampler(SamplerType::LinearRepeat));
			ResourceManager::StoreMaterial(newMaterial);
		}

		for (auto& mesh : importResult.Meshes)
		{
			Entity newEntity = CreateEntity(mesh.ObjectName);
//			newEntity.AddComponent<TransformComponent>(glm::vec3(0.0f, 0.0f, 0.0f));
			newEntity.AddComponent<TransformComponent>(mesh.Transform);

			newEntity.AddComponent<MeshComponent>(mesh.VertexList, mesh.IndexList);
			newEntity.AddComponent<MaterialComponent>(ResourceManager::GetMaterialByName(mesh.MaterialName)->GetID());
		}
	}

	void Scene::ImportOBJModel(const std::string& filepath)
	{
		ImportResult importResult = ObjLoader::Import(filepath);

		if (!importResult.HasMeshData)
		{
			return;
		}

		for (auto& material : importResult.Materials)
		{
			Ref<Material> newMaterial = PBRMaterial::Create(material, ResourceManager::GetShader(ShaderType::Default), ResourceManager::GetSampler(SamplerType::LinearRepeat));
			ResourceManager::StoreMaterial(newMaterial);
		}

		for (auto& mesh : importResult.Meshes)
		{
			Entity newEntity = CreateEntity(mesh.ObjectName);
			newEntity.AddComponent<TransformComponent>(glm::vec3(0.0f, 0.0f, 0.0f));

			newEntity.AddComponent<MeshComponent>(mesh.VertexList, mesh.IndexList);
			newEntity.AddComponent<MaterialComponent>(ResourceManager::GetMaterialByName(mesh.MaterialName)->GetID());
		}

		VirtualFileSystem::PrintStructure();
	}

	void Scene::SaveScene(const std::string& sceneFilepath)
	{
		std::unordered_map<UUID, Ref<Material>> materialsToWrite;
		std::unordered_map<std::string, Ref<Vulkan::Texture2D>> texturesToWrite;

		Serializer sceneSerializer(sceneFilepath);

		SerializerMagic magic = SerializerMagic::Scene;
		sceneSerializer.Write<SerializerMagic>((const char*)&magic);

		{
			for (auto& entity : m_SceneEntities)
			{
				SerializerTag tag = SerializerTag::Entity;
				sceneSerializer.Write<SerializerTag>((const char*)&tag);

				UUID entityID = entity.ID();
				sceneSerializer.Write<UUID>((const char*)&entityID);

				TagComponent& tagComponent = entity.GetComponent<TagComponent>();
				sceneSerializer.Write<std::string>(tagComponent.Tag.c_str(), (uint32_t)tagComponent.Tag.size());

				if (entity.HasComponent<TransformComponent>())
				{
					tag = SerializerTag::TransformComponent;
					sceneSerializer.Write<SerializerTag>((const char*)&tag);

					TransformComponent& transformComponent = entity.GetComponent<TransformComponent>();
					sceneSerializer.Write<glm::vec3>((const char*)&transformComponent.Translation);
					sceneSerializer.Write<glm::vec3>((const char*)&transformComponent.Rotation);
					sceneSerializer.Write<glm::vec3>((const char*)&transformComponent.Scale);
				}

				if (entity.HasComponent<LightComponent>())
				{
					tag = SerializerTag::LightComponent;
					sceneSerializer.Write<SerializerTag>((const char*)&tag);

					LightComponent& lightComponent = entity.GetComponent<LightComponent>();
					sceneSerializer.Write<glm::vec3>((const char*)&lightComponent.Position);
					sceneSerializer.Write<glm::vec3>((const char*)&lightComponent.Color);
				}

				if (entity.HasComponent<MaterialComponent>())
				{
					tag = SerializerTag::MaterialComponent;
					sceneSerializer.Write<SerializerTag>((const char*)&tag);

					MaterialComponent& materialComponent = entity.GetComponent<MaterialComponent>();
					sceneSerializer.Write<MaterialType>((const char*)&materialComponent.Type);
					sceneSerializer.Write<UUID>((const char*)&materialComponent.MaterialID);

					materialsToWrite[materialComponent.MaterialID] = ResourceManager::GetMaterial(materialComponent.MaterialID);
				}

				if (entity.HasComponent<MeshComponent>())
				{
					tag = SerializerTag::MeshComponent;
					sceneSerializer.Write<SerializerTag>((const char*)&tag);

					MeshComponent& meshComponent = entity.GetComponent<MeshComponent>();

					uint32_t vertexCount = (uint32_t)meshComponent.VertexList.size();
					sceneSerializer.Write<uint32_t>((const char*)&vertexCount);
					sceneSerializer.WriteVector<std::vector<Vertex>>((const char*)meshComponent.VertexList.data(), (uint32_t)meshComponent.VertexList.size() * sizeof(Vertex));

					uint32_t indexCount = (uint32_t)meshComponent.IndexList.size();
					sceneSerializer.Write<uint32_t>((const char*)&indexCount);
					sceneSerializer.WriteVector<std::vector<uint32_t>>((const char*)meshComponent.IndexList.data(), (uint32_t)meshComponent.IndexList.size() * sizeof(uint32_t));
				}
			}


		}


		{

			for (auto& material : materialsToWrite)
			{
				LOG_CORE_TRACE("Writing material {}", material.second->GetName());

				SerializerTag tag = SerializerTag::Material;
				sceneSerializer.Write<SerializerTag>((const char*)&tag);

				UUID materialID = material.second->GetID();
				sceneSerializer.Write<UUID>((const char*)&materialID);

				std::string materialName = material.second->GetName();
				sceneSerializer.Write<std::string>(materialName.data(), (uint32_t)materialName.size());

				SamplerType samplerType = material.second->GetSamplerType();
				sceneSerializer.Write<SamplerType>((const char*)&samplerType);

				material.second->Serialize(sceneSerializer);

				if (material.second->GetType() == MaterialType::StaticPBR)
				{
					PBRMaterialData* materialData = (PBRMaterialData*)material.second->GetMaterialData();

					texturesToWrite[materialData->AlbedoTexture.TextureName] = materialData->AlbedoTexture.Texture;
					texturesToWrite[materialData->NormalMap.TextureName] = materialData->NormalMap.Texture;
					texturesToWrite[materialData->MetalnessMap.TextureName] = materialData->MetalnessMap.Texture;
					texturesToWrite[materialData->RoughnessMap.TextureName] = materialData->RoughnessMap.Texture;
					texturesToWrite[materialData->AmbientOcclusionMap.TextureName] = materialData->AmbientOcclusionMap.Texture;
				}
			}
		}

		{
			for (auto& texture : texturesToWrite)
			{
				LOG_CORE_TRACE("Writing texture {}  ({})", texture.second->GetTextureName(), texture.second->GetTexturePath());

				SerializerTag tag = SerializerTag::Texture;
				sceneSerializer.Write<SerializerTag>((const char*)&tag);

				sceneSerializer.Write<std::string>((const char*)texture.first.c_str(), (uint32_t)texture.first.size());
				sceneSerializer.Write<std::string>((const char*)texture.second->GetTexturePath().c_str(), (uint32_t)texture.second->GetTexturePath().size());

				FileSystemItem* item = VirtualFileSystem::GetItem(texture.second->GetTexturePath());

				if (item != nullptr)
				{
					sceneSerializer.Write<std::string>((const char*)item->Name.c_str(), (uint32_t)item->Name.size());
					sceneSerializer.Write<FileItemType>((const char*)&item->Type);
					sceneSerializer.Write<bool>((const char*)&item->SystemFile);
					sceneSerializer.Write<uint32_t>((const char*)&item->DataSize);
					sceneSerializer.WriteVector<std::vector<uint8_t>>((const char*)item->Data.data(), item->DataSize * sizeof(uint8_t));
				}
			}
		}


		SerializerTag tag = SerializerTag::EndOfFile;
		sceneSerializer.Write<SerializerTag>((const char*)&tag);
	}


	void Scene::LoadScene(const std::string& sceneFilepath)
	{
//		std::unordered_map<UUID, Ref<Material>> materialsToWrite;
//		std::unordered_map<std::string, Ref<Vulkan::Texture2D>> texturesToWrite;

		Deserializer sceneDeserializer(sceneFilepath);

		SerializerMagic magic = sceneDeserializer.Read<SerializerMagic>();

		if (magic != SerializerMagic::Scene)
		{
			LOG_CORE_ERROR("[Scene] {} is not a valid scene file", sceneFilepath);
			return;
		}

		ClearScene(false);

		SerializerTag tag;
		UUID activeEntityID = 0;

		std::vector<Ref<PBRMaterial>> pbrMaterials;

		while ((tag = sceneDeserializer.Read<SerializerTag>()) != SerializerTag::EndOfFile)
		{
			if (tag == SerializerTag::Entity)
			{
				activeEntityID = sceneDeserializer.Read<UUID>();
				std::string tagComponentTag = sceneDeserializer.Read<std::string>();

				m_SceneEntities.emplace_back(activeEntityID);
				ECS::EntitySystem::AddComponent<TagComponent>(activeEntityID, tagComponentTag);
			}

			if (tag == SerializerTag::TransformComponent)
			{
				TransformComponent transformComponent;
				transformComponent.Translation = sceneDeserializer.Read<glm::vec3>();
				transformComponent.Rotation = sceneDeserializer.Read<glm::vec3>();
				transformComponent.Scale = sceneDeserializer.Read<glm::vec3>();
				transformComponent.CalculateTransform();

				ECS::EntitySystem::AddComponent<TransformComponent>(activeEntityID, transformComponent);
			}

			if (tag == SerializerTag::LightComponent)
			{
				LightComponent lightComponent;
				lightComponent.Position = sceneDeserializer.Read<glm::vec3>();
				lightComponent.Color = sceneDeserializer.Read<glm::vec3>();

				ECS::EntitySystem::AddComponent<LightComponent>(activeEntityID, lightComponent);
			}

			if (tag == SerializerTag::MaterialComponent)
			{
				MaterialComponent materialComponent;
				materialComponent.Type = sceneDeserializer.Read<MaterialType>();
				materialComponent.MaterialID = sceneDeserializer.Read<UUID>();

				ECS::EntitySystem::AddComponent<MaterialComponent>(activeEntityID, materialComponent);
			}

			if (tag == SerializerTag::MeshComponent)
			{
				MeshComponent meshComponent;

				uint32_t vertexCount = sceneDeserializer.Read<uint32_t>();
				meshComponent.VertexList = sceneDeserializer.ReadVector<std::vector<Vertex>>(vertexCount, sizeof(Vertex));

				uint32_t indexCount = sceneDeserializer.Read<uint32_t>();
				meshComponent.IndexList = sceneDeserializer.ReadVector<std::vector<uint32_t>>(indexCount, sizeof(uint32_t));

				ECS::EntitySystem::AddComponent<MeshComponent>(activeEntityID, meshComponent);
			}

			if (tag == SerializerTag::Material)
			{
				UUID materialID = sceneDeserializer.Read<UUID>();
				std::string materialName = sceneDeserializer.Read<std::string>();

				SamplerType samplerType = sceneDeserializer.Read<SamplerType>();

				Ref<PBRMaterial> newMaterial = CreateRef<PBRMaterial>(materialID, materialName, ResourceManager::GetShader(ShaderType::Default), samplerType);
				newMaterial->Deserialize(sceneDeserializer);

				if (ResourceManager::GetMaterialByName(materialName) == nullptr)
				{
					pbrMaterials.emplace_back(newMaterial);
				}

			}

			if (tag == SerializerTag::Texture)
			{
				std::string textureName = sceneDeserializer.Read<std::string>();
				std::string texturePath = sceneDeserializer.Read<std::string>();

				FileSystemItem newItem;

				newItem.Name = sceneDeserializer.Read<std::string>();
				newItem.Type = sceneDeserializer.Read<FileItemType>();
				newItem.SystemFile = sceneDeserializer.Read<bool>();
				newItem.DataSize = sceneDeserializer.Read<uint32_t>();
				newItem.Data = sceneDeserializer.ReadVector<std::vector<uint8_t>>(newItem.DataSize);

				if (!VirtualFileSystem::FindFile(texturePath))
				{
					VirtualFileSystem::AddFile(texturePath, newItem);
				}
			}
		}

		for (auto& pbrMaterial : pbrMaterials)
		{
			pbrMaterial->SetupMaterial();
			ResourceManager::StoreMaterial(pbrMaterial);
		}
	}

}
