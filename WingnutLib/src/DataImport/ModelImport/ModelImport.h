#pragma once

#include "DataImport/ImportResult.h"

#include "Renderer/Renderer.h"


struct aiNode;
struct aiScene;


namespace Wingnut
{



	class ModelImport
	{
	public:
		static ImportResult ImportFBX(const std::string& filepath);

		static void GetNodeData(const aiScene* scene, aiNode* node, ImportResult& importResult, glm::mat4 transform);

	private:
		static void CalculateTangentAndBitangent(std::vector<Vertex>& vertexList, uint32_t index0, uint32_t index1, uint32_t index2);
	};

}
