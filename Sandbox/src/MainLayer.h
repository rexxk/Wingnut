#pragma once


#include "Wingnut.h"


class MainLayer : public Wingnut::Layer
{
public:
	MainLayer(const std::string& name);
	virtual ~MainLayer();

	virtual void OnAttach() override;
	virtual void OnUpdate() override;

};
