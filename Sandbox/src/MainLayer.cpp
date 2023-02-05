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

	std::vector<Vertex> quadVertices =
	{
		{ { -0.5f,  0.5f, 0.0f }, { 0.0, 1.0 }, { 1.0f, 1.0f, 1.0f, 1.0f } },
		{ { -0.5f, -0.5f, 0.0f }, { 0.0, 0.0 }, { 1.0f, 1.0f, 1.0f, 1.0f } },
		{ {  0.5f, -0.5f, 0.0f }, { 1.0, 0.0 }, { 1.0f, 1.0f, 1.0f, 1.0f } },
		{ {  0.5f,  0.5f, 0.0f }, { 1.0, 1.0 }, { 1.0f, 1.0f, 1.0f, 1.0f } },
	};

	std::vector<uint32_t> quadIndices =
	{
		0, 1, 2, 2, 3, 0,
	};

	ShaderStore::LoadShader("basic", "assets/shaders/Basic.shader");

	auto& rendererData = Renderer::GetContext()->GetRendererData();
	auto extent = rendererData.Device->GetDeviceProperties().SurfaceCapabilities.currentExtent;


	m_Camera = Camera::Create(glm::vec3(0.0f, 0.0f, -3.0f), extent.width, extent.height);


	SceneProperties sceneProperties;
	sceneProperties.SceneExtent= rendererData.Device->GetDeviceProperties().SurfaceCapabilities.currentExtent;
	sceneProperties.SceneCamera = m_Camera;

	m_Scene = Scene::Create(sceneProperties);



	UUID entity = m_Scene->CreateEntity("Entity");
	std::string tag = ECS::EntitySystem::GetComponent<TagComponent>(entity).Tag;

	LOG_TRACE("Entity '{}' created: {}", tag, entity);

	ECS::EntitySystem::AddComponent<MeshComponent>(entity, quadVertices, quadIndices);

	m_Scene->CreateUISceneImageDescriptor(Wingnut::ImGuiContext::Get().GetSampler());


}

void MainLayer::OnDetach()
{
	Renderer::WaitForIdle();

	m_Scene->Release();
}

void MainLayer::OnUpdate(Timestep ts)
{
	m_Scene->Begin();

	m_Scene->Draw();

	m_Scene->End();

}


void MainLayer::OnUIRender()
{

	static uint32_t viewportWidth = 0;
	static uint32_t viewportHeight = 0;

	ImGui::DockSpaceOverViewport(nullptr); //, ImGuiDockNodeFlags_AutoHideTabBar);

	ImGui::ShowDemoWindow();

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

		ImGui::Image((ImTextureID)m_Scene->GetSceneImageDescriptor()->GetDescriptor(), windowSize);

	ImGui::End();

	ImGui::PopStyleVar();

}
