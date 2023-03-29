#pragma once

#include "DataImport/ImportResult.h"

#include "Renderer/Renderer.h"


namespace Wingnut
{

	class ObjLoader
	{
	public:
		static ImportResult Import(const std::string& filename);

	private:
		static void CalculateTangentAndBitangent(ImportMesh& mesh, uint32_t index0, uint32_t index1, uint32_t index2);

		static void LoadMaterialFile(const std::string& filename, ImportResult& importResult);
	};


}
