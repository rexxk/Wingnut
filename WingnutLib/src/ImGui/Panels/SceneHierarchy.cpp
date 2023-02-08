#include "wingnut_pch.h"
#include "SceneHierarchy.h"

#include <imgui.h>



namespace Wingnut
{

	SceneHierarchy::SceneHierarchy(Ref<Scene> scene)
		: m_Scene(scene)
	{

	}

	SceneHierarchy::~SceneHierarchy()
	{

	}

	void SceneHierarchy::Draw()
	{
		ImGui::Begin("Scene Hierarchy");

		for (auto& uuid : m_Scene->GetEntityList())
		{
			DrawEntity(uuid);
		}
		

		ImGui::End();
	}

	void SceneHierarchy::DrawEntity(UUID uuid)
	{

		if (ImGui::TreeNodeEx((void*)(uint32_t)uuid, ImGuiTreeNodeFlags_OpenOnArrow, "%llu", (uint64_t)uuid))
		{

			ImGui::TreePop();
		}

	}

}
