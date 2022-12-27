#pragma once


namespace Wingnut
{

	std::vector<std::string> Tokenize(const std::string& str, char delimiter);

	std::string RemoveCharacters(const std::string& str, const std::string& charactersToRemove);

}


