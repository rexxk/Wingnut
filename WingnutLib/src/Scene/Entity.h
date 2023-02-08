#pragma once

#include "ECS/ECS.h"


namespace Wingnut
{


	class Entity
	{
	public:
		Entity(UUID entityID);
		~Entity();

		UUID ID() const { return m_EntityID; }

		operator uint64_t() { return m_EntityID; }

		template<typename T, typename ... Args>
		void AddComponent(Args&&... args)
		{
			ECS::EntitySystem::AddComponent<T>(m_EntityID, std::forward<Args&&>(args)...);
		}

		template<typename T>
		void RemoveComponent()
		{
			ECS::EntitySystem::RemoveComponent<T>(m_EntityID);
		}

		template<typename T>
		bool HaveComponent()
		{
			return ECS::EntitySystem::HaveComponent<T>(m_EntityID);
		}

		template<typename T>
		T& GetComponent()
		{
			return ECS::EntitySystem::GetComponent<T>(m_EntityID);
		}

	private:
		UUID m_EntityID;

	};


}
