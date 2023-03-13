#include "MainLayer.h"

#include "ImGui/ImGuiContext.h"

#include <glm/gtc/type_ptr.hpp>



using namespace Wingnut;




MainLayer::MainLayer(const std::string& name)
	: Layer(name)
{

}

MainLayer::~MainLayer()
{

}


void MainLayer::OnAttach()
{
	LOG_TRACE("Attaching Main layer");

/*	std::vector<Vertex> quadVertices =
	{
		{ { -0.5f,  0.5f, 0.0f }, { 0.0, 1.0 }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } },
		{ { -0.5f, -0.5f, 0.0f }, { 0.0, 0.0 }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } },
		{ {  0.5f, -0.5f, 0.0f }, { 1.0, 0.0 }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } },
		{ {  0.5f,  0.5f, 0.0f }, { 1.0, 1.0 }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } },
	};

	std::vector<uint32_t> quadIndices =
	{
		0, 1, 2, 2, 3, 0,
	};
*/



//	m_TextureDescriptor = Vulkan::Descriptor::Create(rendererData.Device, ShaderStore::GetShader("basic"), m_ImageSampler, TextureDescriptor, 0, m_Texture);


	auto& rendererData = Renderer::GetContext()->GetRendererData();
	auto extent = rendererData.Device->GetDeviceProperties().SurfaceCapabilities.currentExtent;


	m_Camera = Camera::Create(glm::vec3(0.0f, 0.0f, -8.0f), extent.width, extent.height);

	SceneProperties sceneProperties;
	sceneProperties.SceneExtent= rendererData.Device->GetDeviceProperties().SurfaceCapabilities.currentExtent;
	sceneProperties.SceneCamera = m_Camera;

	m_Scene = Scene::Create(sceneProperties);
//	Entity cameraEntity = m_Scene->CreateEntity("Camera entity");



//	Ref<Vulkan::Texture2D> defaultTexture = Vulkan::Texture2D::Create("assets/textures/checkerboard.png", Vulkan::TextureFormat::R8G8B8A8_Normalized);
//	TextureStore::AddTexture(defaultTexture);

//	MaterialData materialData;
//	materialData.AlbedoTexture.Texture = defaultTexture;

//	Ref<Material> defaultMaterial = Material::Create("Default", m_Scene->GetShader());
//	MaterialStore::StoreMaterial(defaultMaterial);


/*
	Ref<Vulkan::Texture2D> selfieTexture = Vulkan::Texture2D::Create("assets/textures/selfie.jpg", Vulkan::TextureFormat::R8G8B8A8_Normalized);
	TextureStore::AddTexture(selfieTexture);

	MaterialData selfieMaterialData;
	selfieMaterialData.AlbedoTexture.Texture = selfieTexture;

	Ref<Material> selfieMaterial = Material::Create("selfie", m_Scene->GetShader());
	MaterialStore::StoreMaterial(selfieMaterial);

	Ref<Vulkan::Texture2D> texture = Vulkan::Texture2D::Create("assets/textures/texture.jpg", Vulkan::TextureFormat::R8G8B8A8_Normalized);
	TextureStore::AddTexture(texture);

	MaterialData textureMaterialData;
	textureMaterialData.AlbedoTexture.Texture = texture;

	Ref<Material> textureMaterial = Material::Create("texture", m_Scene->GetShader());
	MaterialStore::StoreMaterial(textureMaterial);
*/
	
//	{
//		Entity cubeEntity = m_Scene->ImportOBJModel("assets/models/sandcube.obj");
//		cubeEntity.AddComponent<TransformComponent>(glm::vec3(-1.0f, 0.0f, 0.0f));
//		cubeEntity.AddComponent<MaterialComponent>(selfieMaterial->GetID());
//	}

//	{
//		m_Scene->ImportOBJModel("assets/models/bugatti.obj");
// 		m_Scene->ImportOBJModel("assets/models/cottage_obj.obj");
//		m_Scene->ImportOBJModel("assets/models/fnscarl.obj");
//		m_Scene->ImportOBJModel("assets/models/holidaybeach.obj");
//		m_Scene->ImportOBJModel("assets/models/leeenfieldmk1.obj");
//		m_Scene->ImportOBJModel("assets/models/Room1.obj");
//		m_Scene->ImportOBJModel("assets/models/Room_pbr.obj");
//		m_Scene->ImportOBJModel("assets/models/sniperrifle.obj");
//		m_Scene->ImportOBJModel("assets/models/SpaceStation.obj");
//		m_Scene->ImportOBJModel("assets/models/watchtower.obj");
//	}

//	{
//		Entity sphereEntity = m_Scene->ImportOBJModel("assets/models/sphere.obj");
//		sphereEntity.AddComponent<TransformComponent>(glm::vec3(1.0f, 0.0f, 0.0f));

//		if (!sphereEntity.HasComponent<MaterialComponent>())
//		{
//			sphereEntity.AddComponent<MaterialComponent>(defaultMaterial->GetID());
//		}
//		sphereEntity.AddComponent<MaterialComponent>(textureMaterial->GetID());
//	}

/* 
	{
		Entity entity = m_Scene->CreateEntity("Entity");

		entity.AddComponent<MeshComponent>(quadVertices, quadIndices);
		entity.AddComponent<TransformComponent>(glm::vec3(-0.5f, 0.0f, 0.0f));
		entity.AddComponent<MaterialComponent>(selfieMaterial->GetID());
	}

	{
		Entity entity = m_Scene->CreateEntity("Another entity");

		entity.AddComponent<MeshComponent>(quadVertices, quadIndices);
		entity.AddComponent<TransformComponent>(glm::vec3(0.5f, 0.0f, 0.0f));
		entity.AddComponent<MaterialComponent>(textureMaterial->GetID());
	}
*/


	m_MaterialEditorPanel = CreateRef<MaterialEditorPanel>();
	m_MaterialStorePanel = CreateRef<MaterialStorePanel>(m_Scene);
	m_MetricsPanel = CreateRef<MetricsPanel>();
	m_PropertyPanel = CreateRef<PropertyPanel>();
	m_SceneHierarchyPanel = CreateRef<SceneHierarchyPanel>(m_Scene);
	m_TexturePanel = CreateRef<TexturePanel>();

}

void MainLayer::OnDetach()
{
	Renderer::WaitForIdle();

	ResourceManager::ClearMaterials();
	ResourceManager::ClearTextures();

	m_Scene->Release();
}

void MainLayer::OnUpdate(Timestep ts)
{

	m_Scene->Update(ts);



	// Draw

	m_Scene->Begin();

	m_Scene->Draw();

	m_Scene->End();

}


void MainLayer::OnUIRender()
{
	auto& rendererData = Renderer::GetContext()->GetRendererData();

	static uint32_t viewportWidth = 0;
	static uint32_t viewportHeight = 0;

	ImGui::DockSpaceOverViewport(nullptr); //, ImGuiDockNodeFlags_AutoHideTabBar);



	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New scene"))
			{
				m_Scene->ClearScene();
			}

			if (ImGui::MenuItem("Load scene"))
			{
//				m_Scene->LoadScene();
			}

			if (ImGui::MenuItem("Save scene"))
			{
				std::string filename = FileDialog::Save(L"Wingnut Scene file\0*.wscene\0\0", "assets/scenes/");

				if (!filename.empty())
				{
					m_Scene->SaveScene(filename);
				}
			}

			if (ImGui::MenuItem("Import model..."))
			{
				std::string filename = FileDialog::Load(L"Wavefront OBJ file\0*.obj\0\0", "assets/models/");

				if (!filename.empty())
				{
					m_Scene->ClearScene();

					m_Scene->ImportOBJModel(filename);

					m_MaterialStorePanel->UpdateMaterialList();
				}
			}

			ImGui::Separator();
			
			if (ImGui::MenuItem("Exit"))
			{
				Application::Get().Exit();
			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}


	m_MaterialEditorPanel->Draw();
	m_MaterialStorePanel->Draw();
	m_MetricsPanel->Draw();
	m_SceneHierarchyPanel->Draw();
	m_PropertyPanel->Draw();
	m_TexturePanel->Draw();


//	ImGui::ShowDemoWindow();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

		ImVec2 windowSize = ImGui::GetContentRegionAvail();

		if ((uint32_t)windowSize.x != viewportWidth || (uint32_t)windowSize.y != viewportHeight)
		{
			Ref<Wingnut::UIViewportResizedEvent> event = CreateRef<Wingnut::UIViewportResizedEvent>((uint32_t)windowSize.x, (uint32_t)windowSize.y);
			AddEventToQueue(event);

			viewportWidth = (uint32_t)windowSize.x;
			viewportHeight = (uint32_t)windowSize.y;
		}

		ImGui::Image((ImTextureID)Renderer::GetContext()->GetRendererData().SceneImage->GetDescriptor(), windowSize);

		if (ImGui::IsWindowHovered())
		{
			m_Scene->EnableCamera(true);
		}
		else
		{
			m_Scene->EnableCamera(false);
		}

	ImGui::End();

	ImGui::PopStyleVar();



}
