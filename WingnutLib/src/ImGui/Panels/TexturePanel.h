#pragma once


namespace Wingnut
{

	class TexturePanel
	{
	public:
		TexturePanel();
		~TexturePanel();

		void Draw();

	private:

		uint32_t m_HorizontalTextureCount = 1;

	};


}
