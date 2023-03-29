#pragma once

#include "DataImport/ImportResult.h"

#include "Renderer/Renderer.h"


namespace Wingnut
{


	class ModelImport
	{
	public:
		static ImportResult ImportFBX(const std::string& filepath);

	private:
		static void CalculateTangentAndBitangent(std::vector<Vertex>& vertexList, uint32_t index0, uint32_t index1, uint32_t index2);
	};

}
