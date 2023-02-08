#include "wingnut_pch.h"
#include "Scene.h"

#include "Assets/ShaderStore.h"

#include "Event/EventUtils.h"
#include "Event/WindowEvents.h"

#include "Components.h"
#include "Entity.h"

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
		m_ImageSampler = Vulkan::ImageSampler::Create(rendererData.Device, Vulkan::ImageSamplerFilter::Linear, Vulkan::ImageSamplerMode::Repeat);

//		m_Texture = Vulkan::Texture2D::Create("assets/textures/texture.jpg", Vulkan::TextureFormat::R8G8B8A8_Normalized);
		m_Texture = Vulkan::Texture2D::Create("assets/textures/selfie.jpg", Vulkan::TextureFormat::R8G8B8A8_Normalized);
		m_TextureDescriptor = Vulkan::Descriptor::Create(rendererData.Device, ShaderStore::GetShader("basic"), m_ImageSampler, TextureDescriptor, 0, m_Texture);

		m_CameraDataBuffer = Vulkan::UniformBuffer::Create(rendererData.Device, sizeof(CameraData));
		m_CameraDescriptor = Vulkan::Descriptor::Create(rendererData.Device, ShaderStore::GetShader("basic"), CameraDescriptor, 0, m_CameraDataBuffer);

		m_EntityRegistry = CreateRef<ECS::Registry>();
		m_EntitySystem = CreateRef<ECS::EntitySystem>(m_EntityRegistry);


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

		m_Texture->Release();

		if (m_ImageSampler)
		{
			m_ImageSampler->Release();
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
		m_SceneRenderer->SubmitDescriptor(m_TextureDescriptor);

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

			m_SceneRenderer->SubmitToDrawList(entity, meshComponent.VertexList, meshComponent.IndexList);
		}

		m_SceneRenderer->Draw();

	}


	UUID Scene::CreateEntity(const std::string& tag)
	{
		UUID id = m_EntitySystem->Create();
		ECS::EntitySystem::AddComponent<TagComponent>(id, tag);

		return id;
	}

	const std::unordered_set<UUID>& Scene::GetEntityList()
	{
		return m_EntityRegistry->GetRegistry();
	}

	void Scene::CreateUISceneImageDescriptor(Ref<Vulkan::ImageSampler> sampler)
	{
		auto& rendererData = Renderer::GetContext()->GetRendererData();

		m_RendererImageDescriptor = Vulkan::Descriptor::Create(rendererData.Device, ShaderStore::GetShader("ImGui"), sampler, ImGuiTextureDescriptor, 0, rendererData.SceneImage);
	}


	void Scene::Update(Timestep ts)
	{
		m_SceneCamera->Update(ts);
	}


}
