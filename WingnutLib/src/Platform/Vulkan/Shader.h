#pragma once



namespace Wingnut
{

	namespace Vulkan
	{


		enum class ShaderDomain
		{
			None,
			Vertex,
			Fragment,

//			Raytrace,

//			Compute,
		};


		class Shader
		{
		public:
			Shader(const std::string& shaderPath, ShaderDomain domain);
			~Shader();

			void Reload();

		private:

			void LoadSource();

			void Compile();

			void Reflect();


		private:

			std::string m_ShaderPath;
			ShaderDomain m_Domain;

			std::vector<uint32_t> m_Data;
			std::vector<uint8_t> m_Source;

		};


	}

}
