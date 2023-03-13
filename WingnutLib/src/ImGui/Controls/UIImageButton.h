#pragma once

#include "Renderer/Material.h"


namespace Wingnut
{


	class UIImageButton
	{
	public:
		UIImageButton(MaterialTextureType materialTextureType, Ref<Material> material);

		void Draw();

	private:
		MaterialTextureType m_MaterialTextureType;

		Ref<Material> m_Material = nullptr;
	};


}
