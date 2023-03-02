#pragma once

#include <glm/glm.hpp>


namespace Wingnut
{


	class Vec3Control
	{
	public:
		Vec3Control(const std::string& label, glm::vec3& value, float minValue = 0.0f, float maxValue = 0.0f, float step = 0.1f, float resetValue = 0.0f, float columnWidth = 100.0f);

		void Draw();

	private:

		std::string m_Label;

		glm::vec3& m_Value;

		float m_MinValue;
		float m_MaxValue;
		float m_Step;

		float m_ResetValue = 0.0f;
		float m_ColumnWidth = 100.0f;

	};



}
