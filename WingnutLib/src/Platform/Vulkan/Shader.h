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

			std::string m_ShaderPath;

		};


	}

}
