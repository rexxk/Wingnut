#include "wingnut_pch.h"
#include "Scene.h"

#include "Assets/ResourceManager.h"

#include "Core/Timer.h"

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

		m_CameraUB = Vulkan::UniformBuffer::Create(rendererData.Device, sizeof(CameraData));
		m_CameraDescriptor = Vulkan::Descriptor::Create(rendererData.Device, ResourceManager::GetShader(ShaderType::Default), CameraDescriptor);
		m_CameraDescriptor->SetBufferBinding(0, m_CameraUB);
		m_CameraDescriptor->UpdateBindings();

		m_LightUB = Vulkan::UniformBuffer::Create(rendererData.Device, sizeof(LightData));
		m_LightDescriptor = Vulkan::Descriptor::Create(rendererData.Device, ResourceManager::GetShader(ShaderType::Default), LightsDescriptor);
		m_LightDescriptor->SetBufferBinding(0, m_LightUB);
		m_LightDescriptor->UpdateBindings();

		m_EntityRegistry = CreateRef<ECS::Registry>();


		SubscribeToEvent<WindowResizedEvent>([&](WindowResizedEvent& event)
			{
				auto& rendererData = Renderer::GetContext()->GetRendererData();
				m_RendererImageDescriptor->SetImageBinding(0, rendererData.SceneImage, ResourceManager::GetSampler(SamplerType::Default));
				m_RendererImageDescriptor->UpdateBindings();

				return false;
			});


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


	void Scene::CreateUISceneImageDescriptor(Ref<Vulkan::ImageSampler> sampler)
	{
		auto& rendererData = Renderer::GetContext()->GetRendererData();

		m_RendererImageDescriptor = Vulkan::Descriptor::Create(rendererData.Device, ResourceManager::GetShader(ShaderType::ImGui), ImGuiTextureDescriptor);
		m_RendererImageDescriptor->SetImageBinding(0, rendererData.SceneImage, ResourceManager::GetSampler(SamplerType::Default));
		m_RendererImageDescriptor->UpdateBindings();
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
			Ref<Material> newMaterial = Material::Create(material, ResourceManager::GetShader(ShaderType::Default), ResourceManager::GetSampler(SamplerType::LinearRepeat));
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

}
