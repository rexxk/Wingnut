#pragma once


#include "wingnut_pch.h"

#include "Core/UUID.h"



namespace Wingnut 
{

	namespace ECS
	{

		template<typename T>
		class ComponentContainer;

		template<typename T>
		class View;

		class Registry
		{
			friend class EntitySystem;

		public:
			const std::unordered_set<UUID>& GetRegistry() { return m_Registry; }

		private:
			UUID GetUUID()
			{
				UUID newUUID = UUID();
				m_Registry.insert(newUUID);

				return newUUID;
			}

			void Remove(UUID uuid)
			{
				auto location = m_Registry.find(uuid);

				if (location != m_Registry.end())
				{
					m_Registry.erase(location);
				}
			}

		private:
			std::unordered_set<UUID> m_Registry;
		};


		class EntitySystem
		{
		public:
			static const uint64_t Null = 0;

			EntitySystem(Ref<Registry> registry)
				: m_Registry(registry)
			{

			}

			static UUID Create(Ref<Registry> registry)
			{
				UUID uuid = registry->GetUUID();

				return uuid;
			}

			UUID Create()
			{
				UUID uuid = m_Registry->GetUUID();

				return uuid;
			}

			void Remove(UUID uuid)
			{
				m_Registry->Remove(uuid);
			}

			template<typename T, typename ... Args>
			static void AddComponent(UUID uuid, Args&& ... args)
			{
				if (EntitySystem::HasComponent<T>(uuid))
				{
					LOG_CORE_WARN("[ECS] Entity {} already have component", uuid);
					return;
				}

				ComponentContainer<T>::Add(uuid, T(std::forward<Args&&>(args)...));
				View<T>::Add(uuid);
			}

			template<typename T>
			static void RemoveComponent(UUID uuid)
			{
				if (EntitySystem::HasComponent<T>(uuid))
				{
					ComponentContainer<T>::Remove(uuid);
					View<T>::Remove(uuid);
				}
			}

			template<typename T>
			static bool HasComponent(UUID uuid)
			{
				return View<T>::Find(uuid);
			}

			template<typename T>
			static T& GetComponent(UUID uuid)
			{
				return ComponentContainer<T>::GetComponent(uuid);
			}

			template<typename T>
			static const std::vector<UUID> GetView()
			{
				return View<T>::Get();
			}

		private:
			Ref<Registry> m_Registry;
		};


		template<typename T>
		class ComponentContainer
		{
		public:
			static void Add(UUID uuid, T&& component)
			{
				if (s_Instance == nullptr)
				{
					s_Instance = new ComponentContainer();
				}

				s_Instance->m_Components.try_emplace(uuid, component);
			}

			static void Remove(UUID uuid)
			{
				auto location = s_Instance->m_Components.find(uuid);

				if (location != s_Instance->m_Components.end())
				{
					s_Instance->m_Components.erase(location);
				}
			}

			static T& GetComponent(UUID uuid)
			{
				if (s_Instance)
				{
					return s_Instance->m_Components[uuid];
				}

				return *(T*)nullptr;
			}

		private:
			inline static ComponentContainer* s_Instance = nullptr;

			std::unordered_map<UUID, T> m_Components;
		};


		template<typename T>
		class View
		{
		public:
			static void Add(UUID uuid)
			{
				if (s_Instance == nullptr)
				{
					s_Instance = new View();
				}
				
				s_Instance->m_Entities.push_back(uuid);
			}

			static void Remove(UUID uuid)
			{
				if (s_Instance)
				{
					auto location = std::find(s_Instance->m_Entities.begin(), s_Instance->m_Entities.end(), uuid);

					if (location != s_Instance->m_Entities.end())
					{
						s_Instance->m_Entities.erase(location);
					}
				}
			}

			static bool Find(UUID uuid)
			{
				if (s_Instance != nullptr)
				{
					return std::find(s_Instance->m_Entities.begin(), s_Instance->m_Entities.end(), uuid) != s_Instance->m_Entities.end();
				}

				return false;
			}

			static const std::vector<UUID>& Get()
			{
				if (s_Instance == nullptr)
				{
					s_Instance = new View;
				}

				return s_Instance->m_Entities;
			}

		private:
			inline static View* s_Instance = nullptr;

			std::vector<UUID> m_Entities;
		};

	}


}
