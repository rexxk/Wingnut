#include "wingnut_pch.h"
#include "ResourcePanel.h"

#include "Assets/ResourceManager.h"

#include "Core/Application.h"

#include "Renderer/Renderer.h"

#include "File/FileDialog.h"

#include <imgui.h>


namespace Wingnut
{


	ResourcePanel::ResourcePanel()
	{
		m_ActiveDirectory = &VirtualFileSystem::GetRootDirectory();

		m_FileUpTexture = Vulkan::Texture2D::Create("assets/textures/file_up.png", Vulkan::TextureFormat::R8G8B8A8_Normalized, false, true);
		m_FileModelTexture = Vulkan::Texture2D::Create("assets/textures/file_model.png", Vulkan::TextureFormat::R8G8B8A8_Normalized, false, true);
		m_FileSceneTexture = Vulkan::Texture2D::Create("assets/textures/file_scene.png", Vulkan::TextureFormat::R8G8B8A8_Normalized, false, true);
		m_FileDirectoryTexture = Vulkan::Texture2D::Create("assets/textures/file_directory.png", Vulkan::TextureFormat::R8G8B8A8_Normalized, false, true);
	}

	ResourcePanel::~ResourcePanel()
	{
		Release();
	}

	void ResourcePanel::Release()
	{
		if (m_FileUpTexture)
		{
			m_FileUpTexture->Release();
		}

		if (m_FileModelTexture)
		{
			m_FileModelTexture->Release();
		}

		if (m_FileSceneTexture)
		{
			m_FileSceneTexture->Release();
		}

		if (m_FileDirectoryTexture)
		{
			m_FileDirectoryTexture->Release();
		}
	}

	void ResourcePanel::Draw()
	{
		auto& rendererData = Renderer::GetContext()->GetRendererData();

		float paddingSize = 8.0f;
		float textureSize = 64.0f;

		ImGui::Begin("Resources");

//			ImGui::Text("Texture panel (%d)", m_HorizontalTextureCount);

			if (ImGui::Button("Load resource"))
			{
#ifdef _WIN32
				wchar_t directory[MAX_PATH];
				std::mbstowcs(directory, Application::Get().GetBaseDirectory().c_str(), MAX_PATH);
				SetCurrentDirectory(directory);
#endif

				std::string filename = FileDialog::Load(L"All files\0*.*\0\0", "assets/textures/");

				if (!filename.empty())
				{
					Ref<Vulkan::Texture2D> newTexture = Vulkan::Texture2D::Create(filename, Vulkan::TextureFormat::R8G8B8A8_Normalized, m_FlipHorizontal);
//					Ref<Vulkan::Descriptor> newDescriptor = Vulkan::Descriptor::Create(rendererData.Device, ShaderStore::GetShader(ShaderType::Default), SamplerStore::GetSampler(SamplerType::Default), TextureDescriptor, AlbedoTextureBinding, newTexture);
					Ref<Vulkan::Descriptor> newDescriptor = Vulkan::Descriptor::Create(rendererData.Device, ResourceManager::GetShader(ShaderType::ImGui), ImGuiTextureDescriptor);
					newDescriptor->SetImageBinding(0, newTexture, ResourceManager::GetSampler(SamplerType::Default));
					newDescriptor->UpdateBindings();

					ResourceManager::AddTexture(newTexture);

//					ResourceManager::AddTextureData(newTexture, newDescriptor);
				}
			}

			ImGui::SameLine();

			ImGui::Checkbox("Flip Horizontal", &m_FlipHorizontal);

			ImGui::Separator();

			ImGui::Text("Active directory: %s", m_ActiveDirectory->Name.c_str());

			ImGui::Separator();

			ImVec2 regionSize = ImGui::GetContentRegionAvail();

			m_HorizontalTextureCount = (uint32_t)(regionSize.x / (float)(textureSize + (paddingSize * 2))) + 1;

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(paddingSize, paddingSize));

			uint32_t itemCount = 0;
			uint32_t rowItem = 0;

			if (m_ActiveDirectory->Parent != nullptr)
			{
				ImGui::BeginGroup();

				if (ImGui::ImageButton((ImTextureID)m_FileUpTexture->GetDescriptor(), ImVec2(textureSize, textureSize)))
				{
					m_ActiveDirectory = m_ActiveDirectory->Parent;
				}

				ImGui::PushItemWidth(textureSize + (paddingSize * 2));
				ImGui::PushTextWrapPos((rowItem++ + 1) * (textureSize + (paddingSize * 2)));
				ImGui::TextWrapped("..");
				ImGui::PopTextWrapPos();
				ImGui::PopItemWidth();

				ImGui::EndGroup();

				if ((itemCount++ + 1) % m_HorizontalTextureCount != 0)
				{
					ImGui::SameLine();
				}
				else
				{
					rowItem = 0;
				}
			}

			for (auto& directory : m_ActiveDirectory->Subdirectories)
			{
				ImGui::BeginGroup();

				if (ImGui::ImageButton((ImTextureID)m_FileDirectoryTexture->GetDescriptor(), ImVec2(textureSize, textureSize)))
				{
					m_ActiveDirectory = &directory;
				}

				ImGui::PushItemWidth(textureSize + (paddingSize * 2));
				ImGui::PushTextWrapPos((rowItem++ + 1) * (textureSize + (paddingSize * 2)));
				ImGui::TextWrapped("%s", directory.Name.c_str());
				ImGui::PopTextWrapPos();
				ImGui::PopItemWidth();

				ImGui::EndGroup();

				if ((itemCount++ + 1) % m_HorizontalTextureCount != 0)
				{
					ImGui::SameLine();
				}
				else
				{
					rowItem = 0;
				}
			}

			for (auto& file : m_ActiveDirectory->Files)
			{
				ImGui::BeginGroup();

				if (file.Type == FileItemType::Model)
				{
					ImGui::Image((ImTextureID)m_FileModelTexture->GetDescriptor(), ImVec2(textureSize, textureSize));
				}
				if (file.Type == FileItemType::Scene)
				{
					ImGui::Image((ImTextureID)m_FileSceneTexture->GetDescriptor(), ImVec2(textureSize, textureSize));
				}
				if (file.Type == FileItemType::Texture)
				{
					if (file.ItemLink != nullptr)
					{
						ImGui::Image((ImTextureID)file.ItemLink, ImVec2(textureSize, textureSize));
					}
				}


				ImGui::PushItemWidth(textureSize + (paddingSize * 2));
				ImGui::PushTextWrapPos((rowItem++ + 1)* (textureSize + (paddingSize * 2)));
				ImGui::TextWrapped("%s", file.Name.c_str());
				ImGui::PopTextWrapPos();
				ImGui::PopItemWidth();

				ImGui::EndGroup();

				if ((itemCount++ + 1) % m_HorizontalTextureCount != 0)
				{
					ImGui::SameLine();
				}
				else
				{
					rowItem = 0;
				}
			}

			/*
			//			for (uint32_t i = 0; i < 16; i++)
			for (auto& textureIterator : ResourceManager::GetTextureContainer())
			{
				UUID textureID = textureIterator.first;

				ImGui::BeginGroup();

				{

					ImGui::Image((ImTextureID)ResourceManager::GetTexture(textureID)->GetDescriptor(), ImVec2(textureSize, textureSize));
//					ImGui::Image((ImTextureID)ResourceManager::GetDescriptor(textureID)->GetDescriptor(), ImVec2(textureSize, textureSize));

					ImGui::PushItemWidth(textureSize + (paddingSize * 2));

					ImGui::PushTextWrapPos((rowItem++ + 1) * (textureSize + (paddingSize * 2)));
					ImGui::TextWrapped("%s", ResourceManager::GetTexture(textureID)->GetTextureName().c_str());
//					ImGui::Text("%s", TextureStore::GetTexture(textureID)->GetTextureName().c_str());
					ImGui::PopTextWrapPos();

					ImGui::PopItemWidth();
				}


				ImGui::EndGroup();

				if ((itemCount++ + 1) % m_HorizontalTextureCount != 0)
				{
					ImGui::SameLine();
				}
				else
				{
					rowItem = 0;
				}

				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
				{
					// Should send texture ID, and texture ID needs to be implemented.
//					UUID textureID = rendererData.DefaultTexture->GetTextureID();

					ImGui::SetDragDropPayload("TexturePayload", &textureID, sizeof(UUID));

					ImGui::Text("Texture");

					ImGui::EndDragDropSource();
				}

			}

			*/

			ImGui::PopStyleVar();

		ImGui::End();
	}

}
