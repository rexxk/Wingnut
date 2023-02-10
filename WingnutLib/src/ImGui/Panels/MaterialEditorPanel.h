#pragma once


namespace Wingnut
{


	class Material;


	class MaterialEditorPanel
	{
	public:
		MaterialEditorPanel();
		~MaterialEditorPanel();

		void Draw();

	private:
		Ref<Material> m_SelectedMaterial = nullptr;

	};



}
