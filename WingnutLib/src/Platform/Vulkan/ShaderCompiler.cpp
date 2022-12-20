#include "wingnut_pch.h"
#include "ShaderCompiler.h"

#include <glslang/SPIRV/GlslangToSpv.h>



namespace Wingnut
{

	namespace Vulkan
	{

		EShLanguage ShaderDomainToEShLanguage(ShaderDomain domain)
		{
			switch (domain)
			{
			case ShaderDomain::Vertex: return EShLangVertex;
			case ShaderDomain::Fragment: return EShLangFragment;
			}

			return EShLangVertex;
		}

		void InitResources(TBuiltInResource& resources)
		{
			resources.maxLights = 32;
			resources.maxClipPlanes = 6;
			resources.maxTextureUnits = 32;
			resources.maxTextureCoords = 32;
			resources.maxVertexAttribs = 64;
			resources.maxVertexUniformComponents = 4096;
			resources.maxVaryingFloats = 64;
			resources.maxVertexTextureImageUnits = 32;
			resources.maxCombinedTextureImageUnits = 80;
			resources.maxTextureImageUnits = 32;
			resources.maxFragmentUniformComponents = 4096;
			resources.maxDrawBuffers = 32;
			resources.maxVertexUniformVectors = 128;
			resources.maxVaryingVectors = 8;
			resources.maxFragmentUniformVectors = 16;
			resources.maxVertexOutputVectors = 16;
			resources.maxFragmentInputVectors = 15;
			resources.minProgramTexelOffset = -8;
			resources.maxProgramTexelOffset = 7;
			resources.maxClipDistances = 8;
			resources.maxComputeWorkGroupCountX = 65535;
			resources.maxComputeWorkGroupCountY = 65535;
			resources.maxComputeWorkGroupCountZ = 65535;
			resources.maxComputeWorkGroupSizeX = 1024;
			resources.maxComputeWorkGroupSizeY = 1024;
			resources.maxComputeWorkGroupSizeZ = 64;
			resources.maxComputeUniformComponents = 1024;
			resources.maxComputeTextureImageUnits = 16;
			resources.maxComputeImageUniforms = 8;
			resources.maxComputeAtomicCounters = 8;
			resources.maxComputeAtomicCounterBuffers = 1;
			resources.maxVaryingComponents = 60;
			resources.maxVertexOutputComponents = 64;
			resources.maxGeometryInputComponents = 64;
			resources.maxGeometryOutputComponents = 128;
			resources.maxFragmentInputComponents = 128;
			resources.maxImageUnits = 8;
			resources.maxCombinedImageUnitsAndFragmentOutputs = 8;
			resources.maxCombinedShaderOutputResources = 8;
			resources.maxImageSamples = 0;
			resources.maxVertexImageUniforms = 0;
			resources.maxTessControlImageUniforms = 0;
			resources.maxTessEvaluationImageUniforms = 0;
			resources.maxGeometryImageUniforms = 0;
			resources.maxFragmentImageUniforms = 8;
			resources.maxCombinedImageUniforms = 8;
			resources.maxGeometryTextureImageUnits = 16;
			resources.maxGeometryOutputVertices = 256;
			resources.maxGeometryTotalOutputComponents = 1024;
			resources.maxGeometryUniformComponents = 1024;
			resources.maxGeometryVaryingComponents = 64;
			resources.maxTessControlInputComponents = 128;
			resources.maxTessControlOutputComponents = 128;
			resources.maxTessControlTextureImageUnits = 16;
			resources.maxTessControlUniformComponents = 1024;
			resources.maxTessControlTotalOutputComponents = 4096;
			resources.maxTessEvaluationInputComponents = 128;
			resources.maxTessEvaluationOutputComponents = 128;
			resources.maxTessEvaluationTextureImageUnits = 16;
			resources.maxTessEvaluationUniformComponents = 1024;
			resources.maxTessPatchComponents = 120;
			resources.maxPatchVertices = 32;
			resources.maxTessGenLevel = 64;
			resources.maxViewports = 16;
			resources.maxVertexAtomicCounters = 0;
			resources.maxTessControlAtomicCounters = 0;
			resources.maxTessEvaluationAtomicCounters = 0;
			resources.maxGeometryAtomicCounters = 0;
			resources.maxFragmentAtomicCounters = 8;
			resources.maxCombinedAtomicCounters = 8;
			resources.maxAtomicCounterBindings = 1;
			resources.maxVertexAtomicCounterBuffers = 0;
			resources.maxTessControlAtomicCounterBuffers = 0;
			resources.maxTessEvaluationAtomicCounterBuffers = 0;
			resources.maxGeometryAtomicCounterBuffers = 0;
			resources.maxFragmentAtomicCounterBuffers = 1;
			resources.maxCombinedAtomicCounterBuffers = 1;
			resources.maxAtomicCounterBufferSize = 16384;
			resources.maxTransformFeedbackBuffers = 4;
			resources.maxTransformFeedbackInterleavedComponents = 64;
			resources.maxCullDistances = 8;
			resources.maxCombinedClipAndCullDistances = 8;
			resources.maxSamples = 4;
			resources.maxMeshOutputVerticesNV = 256;
			resources.maxMeshOutputPrimitivesNV = 512;
			resources.maxMeshWorkGroupSizeX_NV = 32;
			resources.maxMeshWorkGroupSizeY_NV = 1;
			resources.maxMeshWorkGroupSizeZ_NV = 1;
			resources.maxTaskWorkGroupSizeX_NV = 32;
			resources.maxTaskWorkGroupSizeY_NV = 1;
			resources.maxTaskWorkGroupSizeZ_NV = 1;
			resources.maxMeshViewCountNV = 4;
			resources.limits.nonInductiveForLoops = 1;
			resources.limits.whileLoops = 1;
			resources.limits.doWhileLoops = 1;
			resources.limits.generalUniformIndexing = 1;
			resources.limits.generalAttributeMatrixVectorIndexing = 1;
			resources.limits.generalVaryingIndexing = 1;
			resources.limits.generalSamplerIndexing = 1;
			resources.limits.generalVariableIndexing = 1;
			resources.limits.generalConstantMatrixVectorIndexing = 1;
		}


		void ShaderCompiler::Initialize()
		{
			glslang::InitializeProcess();
		}

		void ShaderCompiler::Shutdown()
		{
			glslang::FinalizeProcess();
		}

		std::pair<ShaderDomain, std::vector<uint32_t>> ShaderCompiler::Compile(const std::string& shaderPath, ShaderDomain domain)
		{
			std::vector<uint32_t> spirvCode;

			std::ifstream sourceFile(shaderPath, std::ios::in | std::ios::binary);

			if (!sourceFile.is_open())
			{
				LOG_CORE_ERROR("[ShaderCompiler] Unable to open file {} for compiling", shaderPath);
				return std::make_pair(ShaderDomain::None, spirvCode);
			}

			LOG_CORE_TRACE("[ShaderCompile] Compiling shader {}", shaderPath);

			sourceFile.seekg(0, sourceFile.end);
			uint32_t fileLength = (uint32_t)sourceFile.tellg();
			sourceFile.seekg(sourceFile.beg);

			std::vector<uint8_t> shaderSource(fileLength);
			sourceFile.read((char*)shaderSource.data(), fileLength);

			shaderSource.emplace_back(0);

			//		LOG_CORE_TRACE("[ShaderCompiler] Data: {}", shaderSource.data());
			sourceFile.close();


			EShLanguage stage = ShaderDomainToEShLanguage(domain);
			glslang::TShader shader(stage);
			glslang::TProgram program;

			TBuiltInResource resources = {};
			InitResources(resources);

			EShMessages messages = (EShMessages)(EShMsgSpvRules | EShMsgVulkanRules);

			const char* shaderStrings[] = { (const char*)shaderSource.data() };

			shader.setStrings(shaderStrings, 1);

			if (!shader.parse(&resources, 100, false, messages))
			{
				LOG_CORE_ERROR("[ShaderCompiler] {}", shader.getInfoLog());
				LOG_CORE_ERROR(" - {}", shader.getInfoDebugLog());

				return std::make_pair(ShaderDomain::None, spirvCode);
			}

			program.addShader(&shader);

			LOG_CORE_TRACE("[ShaderCompile] Linking shader {}", shaderPath);

			if (!program.link(messages))
			{
				LOG_CORE_ERROR("[ShaderCompiler] {}", shader.getInfoLog());
				LOG_CORE_ERROR(" - {}", shader.getInfoDebugLog());

				return std::make_pair(ShaderDomain::None, spirvCode);
			}

			glslang::GlslangToSpv(*program.getIntermediate(stage), spirvCode);

			return std::make_pair(domain, spirvCode);
		}

	}
}
