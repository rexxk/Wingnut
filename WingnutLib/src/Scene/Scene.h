#pragma once


#include "Core/Timestep.h"

#include "ECS/ECS.h"

#include "Platform/Vulkan/Buffer.h"
#include "Platform/Vulkan/Pipeline.h"

#include "Platform/Vulkan/Texture.h"

#include "Renderer/Camera.h"
#include "Renderer/SceneRenderer.h"

#include "Scene/Entity.h"


namespace Wingnut
{

	struct CameraData
	{
		glm::mat4 ViewProjection;
	};

	struct LightData
	{
		glm::vec3 LightPosition;
		float padding;
		glm::vec3 LightColor;
		float padding2;

		glm::vec3 CameraPosition;
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

		Entity CreateEntity(const std::string& tag);
		std::vector<Entity>& GetEntities() { return m_SceneEntities; }

		Ref<Vulkan::Shader> GetShader() { return m_SceneRenderer->GetShader(); }

		void Begin();
		void End();

		void Draw();

		void Update(Timestep ts);

		void EnableCamera(bool enable) { m_SceneCamera->Enable(enable);	}

		void ImportOBJModel(const std::string& filepath);

	private:

		Ref<SceneRenderer> m_SceneRenderer = nullptr;
		
		Ref<Camera> m_SceneCamera = nullptr;
		VkExtent2D m_SceneExtent;

		Ref<Vulkan::UniformBuffer> m_CameraUB = nullptr;
		Ref<Vulkan::Descriptor> m_CameraDescriptor = nullptr;

		Ref<Vulkan::UniformBuffer> m_LightUB = nullptr;
		Ref<Vulkan::Descriptor> m_LightDescriptor = nullptr;

		Ref<ECS::Registry> m_EntityRegistry = nullptr;

		std::vector<Entity> m_SceneEntities;
	};


}
