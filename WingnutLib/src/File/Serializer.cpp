#include "wingnut_pch.h"
#include "Serializer.h"



namespace Wingnut
{

	Serializer::Serializer(const std::string& filepath)
	{
		m_Stream = std::ofstream(filepath, std::ios::binary);

		if (!m_Stream.is_open())
		{
			LOG_CORE_ERROR("[Serializer] Unable to open file {} for writing", filepath);
		}
	}

	Serializer::~Serializer()
	{
		if (m_Stream.is_open())
		{
			m_Stream.flush();

			m_Stream.close();
		}
	}


	Deserializer::Deserializer(const std::string& filepath)
	{
		m_Stream = std::ifstream(filepath, std::ios::binary);

		if (!m_Stream.is_open())
		{
			LOG_CORE_ERROR("[Deserializer] Unable to open file {} for reading", filepath);
		}
	}

	Deserializer::~Deserializer()
	{
		if (m_Stream.is_open())
		{
			m_Stream.close();
		}
	}


}
