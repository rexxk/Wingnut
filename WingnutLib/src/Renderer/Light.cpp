#include "wingnut_pch.h"
#include "Light.h"



namespace Wingnut
{

	Light::Light(const glm::vec3& position, const glm::vec3& color)
		: m_Position(position), m_Color(color)
	{

	}

	Light::~Light()
	{

	}



}
