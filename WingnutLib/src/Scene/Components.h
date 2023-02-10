#pragma once

#include "wingnut_pch.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


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
		glm::vec3 Translation;
		glm::vec3 Rotation;
		glm::vec3 Scale;

		glm::mat4 Transform;

		void CalculateTransform()
		{
			glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), Rotation.x, glm::vec3(1.0f, 0.0f, 0.0f)) *
				glm::rotate(glm::mat4(1.0f), Rotation.y, glm::vec3(0.0f, 1.0f, 0.0f)) *
				glm::rotate(glm::mat4(1.0f), Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

			Transform = glm::translate(glm::mat4(1.0f), Translation) * rotation * glm::scale(glm::mat4(1.0f), Scale);
		}

		TransformComponent()
		{
			Translation = glm::vec3(0.0f);
			Rotation = glm::vec3(0.0f);
			Scale = glm::vec3(1.0f);

			CalculateTransform();
		}

		TransformComponent(const glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale)
			: Translation(translation), Rotation(rotation), Scale(scale)
		{
			CalculateTransform();
		}

		TransformComponent(const glm::mat4& transform)
			: Transform(transform)
		{

		}

		TransformComponent(const TransformComponent& other)
		{
			Translation = other.Translation;
			Rotation = other.Rotation;
			Scale = other.Scale;

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
