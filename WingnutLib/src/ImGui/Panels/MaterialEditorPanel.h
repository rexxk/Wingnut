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
		void DrawPBRMaterial();
		void DrawUIMaterial();

	private:
		Ref<Material> m_SelectedMaterial = nullptr;

		int m_CurrentSamplerSelection = 0;
	};



}
