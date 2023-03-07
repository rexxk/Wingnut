#include "wingnut_pch.h"
#include "UIImageButton.h"

#include "Assets/ResourceManager.h"

#include "Core/Application.h"

#include "Renderer/Renderer.h"

#include "Utils/FileDialog.h"

#include "imgui.h"


namespace Wingnut
{


	UIImageButton::UIImageButton(MaterialType materialType, Ref<Material> material)
		: m_MaterialType(materialType), m_Material(material)
	{
		Draw();
	}


	void UIImageButton::Draw()
	{
		auto& rendererData = Renderer::GetContext()->GetRendererData();

		ImGui::PushID((uint32_t)m_MaterialType);

		UUID textureID;

		if (m_MaterialType == MaterialType::AlbedoTexture)
		{
			textureID = m_Material->GetMaterialData().AlbedoTexture.Texture->GetTextureID();
		}
		else if (m_MaterialType == MaterialType::NormalMap)
		{
			textureID = m_Material->GetMaterialData().NormalMap.Texture->GetTextureID();
		}
		else if (m_MaterialType == MaterialType::MetalnessMap)
		{
			textureID = m_Material->GetMaterialData().MetalnessMap.Texture->GetTextureID();
		}
		else if (m_MaterialType == MaterialType::RoughnessMap)
		{
			textureID = m_Material->GetMaterialData().RoughnessMap.Texture->GetTextureID();
		}
		else if (m_MaterialType == MaterialType::AmbientOcclusionMap)
		{
			textureID = m_Material->GetMaterialData().AmbientOcclusionMap.Texture->GetTextureID();
		}

		if (ImGui::ImageButton((ImTextureID)ResourceManager::GetDescriptor(textureID)->GetDescriptor(), ImVec2(64.0f, 64.0f)))
		{
#ifdef _WIN32
			wchar_t directory[MAX_PATH];
			std::mbstowcs(directory, Application::Get().GetBaseDirectory().c_str(), MAX_PATH);
			SetCurrentDirectory(directory);
#endif

			std::string filename = OpenFileDialog::Open(L"All files\0*.*\0\0", "assets/textures/");

			if (!filename.empty())
			{
				if (!ResourceManager::FindTexture(filename.substr(filename.find_last_of("/\\") + 1)))
				{
					Ref<Vulkan::Texture2D> newTexture = Vulkan::Texture2D::Create(filename, Vulkan::TextureFormat::R8G8B8A8_Normalized, true);

					Ref<Vulkan::Descriptor> newDescriptor = Vulkan::Descriptor::Create(rendererData.Device, ResourceManager::GetShader(ShaderType::ImGui), ImGuiTextureDescriptor);
					newDescriptor->SetImageBinding(0, newTexture, ResourceManager::GetSampler(SamplerType::Default));
					newDescriptor->UpdateBindings();

					ResourceManager::AddTextureData(newTexture, newDescriptor);

					m_Material->SetTexture(m_MaterialType, newTexture);
				}

			}

		}

		ImGui::PopID();

	}


}
