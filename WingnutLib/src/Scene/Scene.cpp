#include "wingnut_pch.h"
#include "Scene.h"

#include "Assets/MaterialStore.h"
#include "Assets/SamplerStore.h"
#include "Assets/ShaderStore.h"

#include "DataImport/Obj/ObjLoader.h"

#include "Event/EventUtils.h"
#include "Event/WindowEvents.h"

#include "Components.h"
#include "Entity.h"

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

		m_CameraDataBuffer = Vulkan::UniformBuffer::Create(rendererData.Device, sizeof(CameraData));
		m_CameraDescriptor = Vulkan::Descriptor::Create(rendererData.Device, ShaderStore::GetShader(ShaderType::Default), CameraDescriptor, 0, m_CameraDataBuffer);

		m_EntityRegistry = CreateRef<ECS::Registry>();


		SubscribeToEvent<WindowResizedEvent>([&](WindowResizedEvent& event)
			{
				auto& rendererData = Renderer::GetContext()->GetRendererData();
				m_RendererImageDescriptor->UpdateDescriptor(rendererData.SceneImage);

				return false;
			});


	}

	Scene::~Scene()
	{

	}

	void Scene::Release()
	{

		if (m_CameraDataBuffer)
		{
			m_CameraDataBuffer->Release();
		}

		if (m_SceneRenderer)
		{
			m_SceneRenderer->Release();
		}

	}

	void Scene::Begin()
	{
		auto& rendererData = Renderer::GetContext()->GetRendererData();
		uint32_t currentFrame = Renderer::GetContext()->GetCurrentFrame();

		m_SceneRenderer->BeginScene(currentFrame);

		CameraData cameraData = {};
		cameraData.ViewProjection = m_SceneCamera->GetViewProjectionMatrix();

		m_CameraDataBuffer->Update(&cameraData, sizeof(CameraData), currentFrame);

		m_SceneRenderer->SubmitDescriptor(m_CameraDescriptor);

	}

	void Scene::End()
	{

		m_SceneRenderer->EndScene();

	}


	void Scene::Draw()
	{
		auto& device = Renderer::GetContext()->GetRendererData().Device;

		auto& entities = ECS::EntitySystem::GetView<MeshComponent>();

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
				material = MaterialStore::GetMaterial(ECS::EntitySystem::GetComponent<MaterialComponent>(entity).MaterialID);
			}

			m_SceneRenderer->SubmitToDrawList(entity, meshComponent.VertexList, meshComponent.IndexList, transform, material);
		}

		m_SceneRenderer->Draw();

	}


	Entity Scene::CreateEntity(const std::string& tag)
	{
		Entity entity(ECS::EntitySystem::Create(m_EntityRegistry));
		entity.AddComponent<TagComponent>(tag);

		m_SceneEntities.emplace_back(entity);

		return entity;
	}


	void Scene::CreateUISceneImageDescriptor(Ref<Vulkan::ImageSampler> sampler)
	{
		auto& rendererData = Renderer::GetContext()->GetRendererData();

		m_RendererImageDescriptor = Vulkan::Descriptor::Create(rendererData.Device, ShaderStore::GetShader(ShaderType::ImGui), sampler, ImGuiTextureDescriptor, 0, rendererData.SceneImage);
	}


	void Scene::Update(Timestep ts)
	{
		m_SceneCamera->Update(ts);
	}


	Entity Scene::ImportOBJModel(const std::string& filepath)
	{
		ObjImportResult importResult = ObjLoader::Import(filepath);

		Entity newEntity = CreateEntity(importResult.ObjectName);

		if (!importResult.IndexList.empty())
		{
			if (importResult.HasMeshData)
			{
				newEntity.AddComponent<MeshComponent>(importResult.VertexList, importResult.IndexList);
			}

			if (importResult.HasMaterial)
			{
				Ref<Material> material = Material::Create(importResult.Material, ShaderStore::GetShader(ShaderType::Default), SamplerStore::GetSampler(SamplerType::LinearRepeat));
				MaterialStore::StoreMaterial(material);

				newEntity.AddComponent<MaterialComponent>(material->GetID());
			}
		}

		return newEntity;
	}

}
