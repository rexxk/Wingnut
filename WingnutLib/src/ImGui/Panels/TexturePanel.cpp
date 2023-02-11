#include "wingnut_pch.h"
#include "TexturePanel.h"

#include "Renderer/Renderer.h"

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

			ImVec2 regionSize = ImGui::GetContentRegionAvail();

			m_HorizontalTextureCount = (uint32_t)(regionSize.x / (float)(textureSize + (paddingSize * 2))) + 1;

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(paddingSize, paddingSize));

			for (uint32_t i = 0; i < 16; i++)
			{
				ImGui::Image((ImTextureID)rendererData.DefaultTextureDescriptor->GetDescriptor(), ImVec2(textureSize, textureSize));

				if ((i + 1) % m_HorizontalTextureCount != 0)
				{
					ImGui::SameLine();
				}

				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
				{
					// Should send texture ID, and texture ID needs to be implemented.
//					ImGui::SetDragDropPayload("TexturePayload", rendererData.DefaultTextureDescriptor, sizeof(VkDescriptorSet));

					ImGui::EndDragDropSource();
				}
			}

			ImGui::PopStyleVar();


		ImGui::End();
	}

}
