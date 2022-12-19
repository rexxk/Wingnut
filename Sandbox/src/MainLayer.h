#pragma once


#include "Wingnut.h"


class MainLayer : public Wingnut::Layer
{
public:
	MainLayer(const std::string& name);
	virtual ~MainLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnUpdate() override;

private:
	Ref<Wingnut::VertexBuffer> m_TriangleVertexBuffer = nullptr;
	Ref<Wingnut::IndexBuffer> m_TriangleIndexBuffer = nullptr;

};
