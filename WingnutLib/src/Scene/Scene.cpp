#include "wingnut_pch.h"
#include "Scene.h"

#include "Components.h"

#include "Renderer/Renderer.h"


namespace Wingnut
{



	Scene::Scene(const SceneProperties& properties)
		: m_Properties(properties)
	{
		auto& rendererData = Renderer::GetContext()->GetRendererData();

		m_SceneRenderer = CreateRef<SceneRenderer>(properties.SceneExtent);

		m_Texture = CreateRef<Vulkan::Texture2D>("assets/textures/texture.jpg");

		m_CameraData = CreateRef<Vulkan::UniformBuffer>(rendererData.Device, sizeof(CameraDescriptorSet));

		m_EntityRegistry = CreateRef<ECS::Registry>();
		m_EntitySystem = CreateRef<ECS::EntitySystem>(m_EntityRegistry);

	}

	Scene::~Scene()
	{

	}

	void Scene::Release()
	{
		if (m_CameraData)
		{
			m_CameraData->Release();
		}

		m_Texture->Release();

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

		m_Properties.SceneCamera->Update();

		CameraDescriptorSet cameraDescriptorSet = {};
		cameraDescriptorSet.ViewProjection = m_Properties.SceneCamera->GetViewProjectionMatrix();

		m_CameraData->Update(&cameraDescriptorSet, sizeof(CameraDescriptorSet), currentFrame);
		m_SceneRenderer->UpdateDescriptor(0, 0, m_CameraData->GetBuffer(currentFrame), sizeof(CameraDescriptorSet));

	}

	void Scene::End()
	{

		m_SceneRenderer->EndScene();

		m_SceneRenderer->Present();
	}


	void Scene::Draw(Ref<Vulkan::VertexBuffer> vertexBuffer, Ref<Vulkan::IndexBuffer> indexBuffer)
	{
		m_SceneRenderer->UpdateDescriptor(2, 0, m_Texture->GetImageView(), m_Texture->GetSampler());

		m_SceneRenderer->Draw(vertexBuffer, indexBuffer);
	}


	UUID Scene::CreateEntity(const std::string& tag)
	{
		UUID newEntity = m_EntitySystem->Create();

		TagComponent tagComponent;
		tagComponent.Tag = tag;

		ECS::EntitySystem::AddComponent<TagComponent>(newEntity, tagComponent);

		return newEntity;
	}


}
