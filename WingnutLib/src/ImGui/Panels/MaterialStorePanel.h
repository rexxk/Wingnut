#pragma once



namespace Wingnut
{

	class MaterialStorePanel
	{
	public:
		MaterialStorePanel();
		~MaterialStorePanel();

		void Draw();

	private:

		int m_CurrentSelection = 0;
		int m_ActiveSelection = -1;
		std::vector<const char*> m_ListboxItems;


	};

}
