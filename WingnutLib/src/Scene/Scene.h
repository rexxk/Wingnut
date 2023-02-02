#pragma once


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

//		Ref<Vulkan::Descriptor> GetRenderImageDescriptor() { return m_Renderer->GetRenderImage(); }


		void Begin();
		void End();

		void Draw();

	private:

		SceneProperties m_Properties;

		Ref<SceneRenderer> m_SceneRenderer = nullptr;

		Ref<Camera> m_SceneCamera = nullptr;

		Ref<Vulkan::UniformBuffer> m_CameraDataBuffer = nullptr;
		Ref<Vulkan::Descriptor> m_CameraDescriptor = nullptr;

		Ref<Vulkan::Texture2D> m_Texture = nullptr;
		Ref<Vulkan::Descriptor> m_TextureDescriptor = nullptr;
		Ref<Vulkan::ImageSampler> m_ImageSampler = nullptr;

		Ref<ECS::Registry> m_EntityRegistry = nullptr;
		Ref<ECS::EntitySystem> m_EntitySystem = nullptr;
	};


}
