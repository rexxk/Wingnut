#include "wingnut_pch.h"
#include "Scene.h"

#include "Assets/ResourceManager.h"

#include "Core/Timer.h"

#include "DataImport/Obj/ObjLoader.h"

#include "Event/EventUtils.h"
#include "Event/WindowEvents.h"

#include "File/Serializer.h"

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

		ClearScene();

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

	void Scene::ClearScene()
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


	Entity Scene::CreateEntity(const std::string& tag)
	{
		Entity entity(ECS::EntitySystem::Create(m_EntityRegistry));
		entity.AddComponent<TagComponent>(tag);

		m_SceneEntities.emplace_back(entity);

		return entity;
	}

	void Scene::Update(Timestep ts)
	{
		m_SceneCamera->Update(ts);
	}


	void Scene::ImportOBJModel(const std::string& filepath)
	{
		ObjImportResult importResult = ObjLoader::Import(filepath);

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
					tag = SerializerTag::Mesh;
					sceneSerializer.Write<SerializerTag>((const char*)&tag);

					MeshComponent& meshComponent = entity.GetComponent<MeshComponent>();
					sceneSerializer.WriteVector<std::vector<Vertex>>((const char*)meshComponent.VertexList.data(), (uint32_t)meshComponent.VertexList.size() * sizeof(Vertex));
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


			}
		}


		SerializerTag tag = SerializerTag::EndOfFile;
		sceneSerializer.Write<SerializerTag>((const char*)&tag);
	}

}
