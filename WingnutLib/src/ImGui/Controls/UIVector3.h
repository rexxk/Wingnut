#pragma once

#include <glm/glm.hpp>


namespace Wingnut
{


	class Vec3Control
	{
	public:
		Vec3Control(const std::string& label, glm::vec3& value, float resetValue = 0.0f, float columnWidth = 100.0f);

		void Draw();

	private:

		std::string m_Label;

		glm::vec3& m_Value;

		float m_ResetValue = 0.0f;
		float m_ColumnWidth = 100.0f;

	};



}
