#pragma once


#include "wingnut_pch.h"



namespace Wingnut
{


	struct TagComponent
	{
		std::string Tag = "<default>";

		TagComponent()
		{

		}

		TagComponent(const std::string& tag)
		{
			Tag = tag;
		}

		TagComponent(const TagComponent& other)
		{
			Tag = other.Tag;
		}

	};


	struct MeshComponent
	{
		std::vector<Vertex> VertexList;
		std::vector<uint32_t> IndexList;


		MeshComponent()
		{

		}

		MeshComponent(const std::vector<Vertex>& vertexList, const std::vector<uint32_t>& indexList)
			: VertexList(vertexList), IndexList(indexList)
		{
		}

		MeshComponent(const MeshComponent& other)
		{
			VertexList = other.VertexList;
			IndexList = other.IndexList;
		}

	};


}
