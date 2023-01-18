#pragma once


#include "wingnut_pch.h"



namespace Wingnut
{


	struct TagComponent
	{
		std::string Tag = "<default>";

		TagComponent()
		{

		}

		TagComponent(const TagComponent& other)
		{
			Tag = other.Tag;
		}

	};



}
