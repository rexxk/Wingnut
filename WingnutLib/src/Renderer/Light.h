#pragma once

#include <glm/glm.hpp>


namespace Wingnut
{

	class Light
	{
	public:
		Light(const glm::vec3& position, const glm::vec3& color);
		~Light();

		glm::vec3& GetPosition() { return m_Position; }
		glm::vec3& GetColor() { return m_Color; }

	private:
		glm::vec3 m_Position;
		glm::vec3 m_Color;

	};

}
