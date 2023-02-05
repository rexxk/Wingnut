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

	m_DebugTexture = Vulkan::Texture2D::Create("assets/textures/texture.jpg", Vulkan::TextureFormat::R8G8B8A8_Normalized);
	m_DebugDescriptor = Vulkan::Descriptor::Create(rendererData.Device, ShaderStore::GetShader("ImGui"), Wingnut::ImGuiContext::Get().GetSampler(), ImGuiTextureDescriptor, 0, m_DebugTexture);


}

void MainLayer::OnDetach()
{
	Renderer::WaitForIdle();


	m_DebugTexture->Release();

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

	ImGui::DockSpaceOverViewport();

	ImGui::ShowDemoWindow();


	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	bool viewportOpen = false;
	ImGui::Begin("Viewport", &viewportOpen, ImGuiWindowFlags_NoTitleBar);


		ImVec2 windowSize = ImGui::GetContentRegionAvail();
		ImGui::Image((ImTextureID)m_Scene->GetSceneImageDescriptor()->GetDescriptor(), windowSize);

	ImGui::End();

	ImGui::PopStyleVar();

}
