#pragma once


#include "Core/Timestep.h"

#include "ECS/ECS.h"

#include "Platform/Vulkan/Buffer.h"
#include "Platform/Vulkan/Pipeline.h"

#include "Platform/Vulkan/Texture.h"

#include "Renderer/Camera.h"
#include "Renderer/SceneRenderer.h"


namespace Wingnut
{

	struct CameraData
	{
		glm::mat4 ViewProjection;
	};



	struct SceneProperties
	{
		VkExtent2D SceneExtent;

		Ref<Camera> SceneCamera;
	};


	class Scene
	{
	public:
		static Ref<Scene> Create(const SceneProperties& sceneProperties);

		Scene(const SceneProperties& properties);
		~Scene();

		void Release();

		UUID CreateEntity(const std::string& tag);

		void CreateUISceneImageDescriptor(Ref<Vulkan::ImageSampler> sampler);

		Ref<Vulkan::Descriptor> GetSceneImageDescriptor() { return m_RendererImageDescriptor; }

		void Begin();
		void End();

		void Draw();

		void Update(Timestep ts);

		void EnableCamera(bool enable) { m_SceneCamera->Enable(enable);	}

		const std::unordered_set<UUID>& GetEntityList();

	private:

		Ref<SceneRenderer> m_SceneRenderer = nullptr;
		
		Ref<Vulkan::Descriptor> m_RendererImageDescriptor = nullptr;

		Ref<Camera> m_SceneCamera = nullptr;
		VkExtent2D m_SceneExtent;

		Ref<Vulkan::UniformBuffer> m_CameraDataBuffer = nullptr;
		Ref<Vulkan::Descriptor> m_CameraDescriptor = nullptr;

		Ref<Vulkan::Texture2D> m_Texture = nullptr;
		Ref<Vulkan::Descriptor> m_TextureDescriptor = nullptr;
		Ref<Vulkan::ImageSampler> m_ImageSampler = nullptr;

		Ref<ECS::Registry> m_EntityRegistry = nullptr;
		Ref<ECS::EntitySystem> m_EntitySystem = nullptr;
	};


}
