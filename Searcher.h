#pragma once
#include <string>
#include <regex>
#include <type_traits>



template<typename Data, size_t SearchColumn>
class Searcher
{
public:
	Searcher(Data& reference)
		:mDataRef(reference)
	{
	
	}

	template<std::enable_if_t < std::is_same_v<typename Data::template elem_t<SearchColumn>, std::string> ,int > = 0>
	auto Search(const std::string& search_for)
	{
			
	}



	std::regex ConstructRegex(const std::string& text)
	{
		std::string temp;
		for (auto& c : text)
		{
			temp += fmt::format("[{:c}|{:c}]", (char)std::tolower(c), (char)std::toupper(c));
		}
		temp += "(?:.*)?";
		return std::move(std::regex(std::move(temp)));
	}

private:
	Data& mDataRef
};