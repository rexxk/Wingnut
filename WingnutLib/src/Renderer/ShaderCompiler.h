#pragma once




namespace Wingnut
{

	enum class ShaderDomain
	{
		Vertex,
		Fragment,
	};


	class ShaderCompiler
	{
	public:
		static void Initialize();
		static void Shutdown();

		static std::vector<uint32_t> Compile(const std::string& shaderPath, ShaderDomain domain);

	};

}
