#pragma once



namespace Wingnut
{

	enum class SerializerTag : uint8_t
	{
		Entity = 0x10,
		Mesh = 0x20,
		Material = 0x30,
		Texture = 0x40,

		TagComponent = 0xA0,
		TransformComponent = 0xA1,
		MeshComponent = 0xA2,
		MaterialComponent = 0xA3,
		LightComponent = 0xA4,

		EndOfFile = 0xFF,
	};


	enum class SerializerMagic : uint32_t
	{
		Scene = 0x80085,
		Material = 0x7175,
	};


	class Serializer
	{
	public:
		Serializer(const std::string& filepath);
		~Serializer();

		template<typename T>
		void Write(const char* data, uint32_t count = 1)
		{
			m_Stream.write(data, sizeof(T) * count);
		}

		template<>
		void Write<std::string>(const char* data, uint32_t count)
		{
			Write<uint32_t>((const char*)&count);
			m_Stream.write(data, count);
		}

		template<typename T>
		void WriteVector(const char* data, uint32_t size = 1)
		{
			m_Stream.write(data, size);
		}

	private:
		std::ofstream m_Stream;
	};


	class Deserializer
	{
	public:
		Deserializer(const std::string& filepath);
		~Deserializer();

		bool EndOfFile() { return m_Stream.eof(); }

		template<typename T>
		T Read(uint32_t count = 1)
		{
			T data;
			m_Stream.read((char*)&data, sizeof(T) * count);

			return data;
		}

		template<>
		std::string Read<std::string>(uint32_t count)
		{
			uint32_t strLength = Read<uint32_t>();

			std::string str;
			str.resize(strLength);

			m_Stream.read(str.data(), strLength);
		}

		template<typename T>
		T ReadVector(uint32_t count = 1, uint32_t size = 1)
		{
			T data;
			data.resize(count);
			m_Stream.read((char*)data.data(), size);

			return data;
		}

		bool IsOpen()
		{
			return m_Stream.is_open();
		}

	private:
		std::ifstream m_Stream;
	};


	class Serializable
	{
	public:
		virtual void Serialize(Serializer& serializer) = 0;
		virtual void Deserialize(Deserializer& deserializer) = 0;
	};

}
