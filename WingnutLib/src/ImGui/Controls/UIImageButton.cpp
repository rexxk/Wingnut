#include "wingnut_pch.h"
#include "UIImageButton.h"

#include "Assets/ResourceManager.h"

#include "Core/Application.h"

#include "Renderer/Renderer.h"

#include "File/FileDialog.h"

#include "imgui.h"


namespace Wingnut
{


	UIImageButton::UIImageButton(MaterialTextureType materialTextureType, Ref<Material> material)
		: m_MaterialTextureType(materialTextureType), m_Material(material)
	{
		Draw();
	}


	void UIImageButton::Draw()
	{
		auto& rendererData = Renderer::GetContext()->GetRendererData();

		ImGui::PushID((uint32_t)m_MaterialTextureType);

		UUID textureID;

		PBRMaterialData* materialData = (PBRMaterialData*)m_Material->GetMaterialData();

		if (m_MaterialTextureType == MaterialTextureType::AlbedoTexture)
		{
			textureID = materialData->AlbedoTexture.Texture->GetTextureID();
		}
		else if (m_MaterialTextureType == MaterialTextureType::NormalMap)
		{
			textureID = materialData->NormalMap.Texture->GetTextureID();
		}
		else if (m_MaterialTextureType == MaterialTextureType::MetalnessMap)
		{
			textureID = materialData->MetalnessMap.Texture->GetTextureID();
		}
		else if (m_MaterialTextureType == MaterialTextureType::RoughnessMap)
		{
			textureID = materialData->RoughnessMap.Texture->GetTextureID();
		}
		else if (m_MaterialTextureType == MaterialTextureType::AmbientOcclusionMap)
		{
			textureID = materialData->AmbientOcclusionMap.Texture->GetTextureID();
		}

		Ref<Vulkan::Texture2D> texture = ResourceManager::GetTexture(textureID);

		if (ImGui::ImageButton((ImTextureID)texture->GetDescriptor(), ImVec2(64.0f, 64.0f)))
		{
#ifdef _WIN32
			wchar_t directory[MAX_PATH];
			std::mbstowcs(directory, Application::Get().GetBaseDirectory().c_str(), MAX_PATH);
			SetCurrentDirectory(directory);
#endif

			std::string filename = FileDialog::Load(L"All files\0*.*\0\0", "assets/textures/");

			if (!filename.empty())
			{
				std::string textureName = filename.substr(filename.find_last_of("/\\") + 1);

				if (!ResourceManager::FindTexture(textureName))
				{
					Ref<Vulkan::Texture2D> newTexture = Vulkan::Texture2D::Create(filename, Vulkan::TextureFormat::R8G8B8A8_Normalized, true, true);

					m_Material->SetTexture(m_MaterialTextureType, newTexture);

					ResourceManager::AddTexture(newTexture);

					VirtualFileSystem::PrintStructure();
				}

			}

		}

		ImGui::PopID();

	}


}
