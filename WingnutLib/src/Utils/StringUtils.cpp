#include "wingnut_pch.h"
#include "StringUtils.h"



namespace Wingnut
{


	std::vector<std::string> Tokenize(const std::string& str, char delimiter)
	{
		std::vector<std::string> tokens;

		size_t offset = 0;
		size_t location = 0;
		

		while ((location = str.find(delimiter, offset)) != std::string::npos)
		{
			std::string token = str.substr(offset, location - offset);

			if (token.size() > 0)
			{
				tokens.emplace_back(token);
			}

			offset = location + 1;
		}

		tokens.emplace_back(str.substr(offset));

		return tokens;
	}

	std::string RemoveCharacters(const std::string& str, const std::string& charactersToRemove)
	{
		std::string cleanedString = str;

		for (size_t i = 0; i < str.length(); i++)
		{
			for (char c : charactersToRemove)
			{
				if (str[i] == c)
				{
					cleanedString[i] = ' ';
				}
			}
		}

		return cleanedString;
	}


}
