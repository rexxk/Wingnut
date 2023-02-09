#pragma once

#include "Scene/Entity.h"


namespace Wingnut
{


	class PropertyPanel
	{
	public:
		PropertyPanel();
		~PropertyPanel();

		void Draw();

	private:
		void DrawTagComponent();
		void DrawMeshComponent();
		void DrawTransformComponent();

	private:
		Entity m_SelectedEntity;

	};


}
