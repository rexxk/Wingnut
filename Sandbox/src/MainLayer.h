#pragma once


#include "Wingnut.h"


using namespace Wingnut::Vulkan;


class MainLayer : public Wingnut::Layer
{
public:
	MainLayer(const std::string& name);
	virtual ~MainLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnUpdate() override;

private:
	Ref<Wingnut::Scene> m_Scene = nullptr;

	Ref<Wingnut::Camera> m_Camera = nullptr;


};
