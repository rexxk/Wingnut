#include "wingnut_pch.h"
#include "TexturePanel.h"

#include "Assets/ShaderStore.h"
#include "Assets/TextureStore.h"

#include "Core/Application.h"

#include "Renderer/Renderer.h"

#include "Utils/FileDialog.h"

#include <imgui.h>


namespace Wingnut
{


	TexturePanel::TexturePanel()
	{

	}

	TexturePanel::~TexturePanel()
	{

	}

	void TexturePanel::Draw()
	{
		auto& rendererData = Renderer::GetContext()->GetRendererData();

		float paddingSize = 8.0f;
		float textureSize = 64.0f;

		ImGui::Begin("Textures");

			ImGui::Text("Texture panel (%d)", m_HorizontalTextureCount);

			if (ImGui::Button("Load texture"))
			{
#ifdef _WIN32
				wchar_t directory[MAX_PATH];
				std::mbstowcs(directory, Application::Get().GetBaseDirectory().c_str(), MAX_PATH);
				SetCurrentDirectory(directory);
#endif

				std::string filename = OpenFileDialog::Open(L"All files\0*.*\0\0", "assets/textures/");

				if (!filename.empty())
				{
					Ref<Vulkan::Texture2D> newTexture = Vulkan::Texture2D::Create(filename, Vulkan::TextureFormat::R8G8B8A8_Normalized);
					Ref<Vulkan::Descriptor> newDescriptor = Vulkan::Descriptor::Create(rendererData.Device, ShaderStore::GetShader(ShaderType::Default), rendererData.DefaultSampler, MaterialDescriptor, 0, newTexture);
					TextureStore::AddTextureData(newTexture, newDescriptor);
				}
			}

			ImGui::Separator();

			ImVec2 regionSize = ImGui::GetContentRegionAvail();

			m_HorizontalTextureCount = (uint32_t)(regionSize.x / (float)(textureSize + (paddingSize * 2))) + 1;

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(paddingSize, paddingSize));

			uint32_t itemCount = 0;

			//			for (uint32_t i = 0; i < 16; i++)
			for (auto& textureIterator : TextureStore::GetTextureContainer())
			{
				UUID textureID = textureIterator.first;

				ImGui::BeginGroup();

//				ImGui::Image((ImTextureID)rendererData.DefaultTextureDescriptor->GetDescriptor(), ImVec2(textureSize, textureSize));
				ImGui::Image((ImTextureID)TextureStore::GetDescriptor(textureID)->GetDescriptor(), ImVec2(textureSize, textureSize));

				ImGui::Text(TextureStore::GetTexture(textureID)->GetTextureName().c_str());

				ImGui::EndGroup();

				if ((itemCount++ + 1) % m_HorizontalTextureCount != 0)
				{
					ImGui::SameLine();
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

			ImGui::PopStyleVar();

		ImGui::End();
	}

}
