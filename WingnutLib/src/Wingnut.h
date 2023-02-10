#pragma once

#include "wingnut_pch.h"

#include "Assets/MaterialStore.h"
#include "Assets/ShaderStore.h"

#include "Core/Application.h"
#include "Core/Timestep.h"

#include "Event/EventBroker.h"
#include "Event/EventUtils.h"
#include "Event/KeyboardEvents.h"
#include "Event/MouseEvents.h"
#include "Event/UIEvents.h"
#include "Event/WindowEvents.h"

#include "ImGui/ImGuiContext.h"

#include "ImGui/Panels/MaterialEditorPanel.h"
#include "ImGui/Panels/MaterialStorePanel.h"
#include "ImGui/Panels/PropertyPanel.h"
#include "ImGui/Panels/SceneHierarchyPanel.h"

#include "Platform/Vulkan/Buffer.h"
#include "Platform/Vulkan/Image.h"
#include "Platform/Vulkan/Shader.h"
#include "Platform/Vulkan/Texture.h"

#include "Renderer/Camera.h"
#include "Renderer/Material.h"
#include "Renderer/Renderer.h"

#include "Scene/Components.h"
#include "Scene/Entity.h"
#include "Scene/Scene.h"

#include <imgui.h>
