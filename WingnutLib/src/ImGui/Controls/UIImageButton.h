#pragma once

#include "Renderer/Material.h"


namespace Wingnut
{


	class UIImageButton
	{
	public:
		UIImageButton(MaterialType type, Ref<Material> material);

		void Draw();

	private:
		MaterialType m_MaterialType;

		Ref<Material> m_Material = nullptr;
	};


}
