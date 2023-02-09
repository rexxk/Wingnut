#pragma once

#include "wingnut_pch.h"

#include <glm/glm.hpp>


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

	struct TransformComponent
	{
		glm::mat4 Transform;

		TransformComponent()
		{
			Transform = glm::mat4(1.0f);
		}

		TransformComponent(const glm::mat4& transform)
			: Transform(transform)
		{

		}

		TransformComponent(const TransformComponent& other)
		{
			Transform = other.Transform;
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

	struct MaterialComponent
	{
		UUID MaterialID;


		MaterialComponent()
		{

		}

		MaterialComponent(UUID materialID)
			: MaterialID(materialID)
		{

		}

		MaterialComponent(const MaterialComponent& other)
		{
			MaterialID = other.MaterialID;
		}

	};


}
