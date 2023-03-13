#include "wingnut_pch.h"
#include "MetricsPanel.h"

#include "Core/Application.h"

#include <imgui.h>


namespace Wingnut
{

	MetricsPanel::MetricsPanel()
	{

	}

	MetricsPanel::~MetricsPanel()
	{

	}

	void MetricsPanel::Draw()
	{
		auto& metrics = Application::Get().GetMetrics();

		ImGui::Begin("Metrics");

		ImGui::Text("FPS: %d", metrics.FPS);
		ImGui::Text("Total frame time: %.3f ms", metrics.TotalFrameTime);
		ImGui::Text("Message handling time: %.3f ms", metrics.MessageHandlingTime);
		ImGui::Text("Layer update time: %.3f ms", metrics.LayerUpdateTime);
		ImGui::Text("Scene draw time: %.3f ms", metrics.SceneDrawTime);
		ImGui::Text("Rendering time: %.3f ms", metrics.RenderingTime);
		ImGui::Text("UI Rendering time: %.3f ms", metrics.UIRenderingTime);
		ImGui::Text("GPU time: %.3f ms", metrics.GPUTime);
		ImGui::Text("Polygon count: %d", metrics.PolygonCount);

		ImGui::End();
	}


}
