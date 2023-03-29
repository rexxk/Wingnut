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


	std::string ConvertFilePathToAssetPath(const std::string& str)
	{
		auto assetLocation = str.find("assets");

		std::string assetString = "";

		if (assetLocation == std::string::npos)
		{
//			return "";
			assetString = "assets/";
			assetLocation = 0;
		}

		assetString += str.substr(assetLocation);

		size_t location = assetString.find("\\");

		while (location != std::string::npos)
		{
			assetString[location] = '/';
//			assetString += assetString.substr(location + 2);

			location = assetString.find("\\");
		}

		return assetString;
	}

	std::string ConvertFilePath(const std::string& str)
	{
		std::string convertedString = str;

		size_t location = convertedString.find("\\");

		while (location != std::string::npos)
		{
			convertedString[location] = '/';

			location = convertedString.find("\\");
		}

		return convertedString;
	}

}
