#pragma once
#include <string>
#include <regex>
#include <type_traits>


template<size_t SearchColumn, typename Data>
class Searcher
{
public:
	Searcher(Data& reference)
		:mDataRef(reference)
	{}

	template<std::enable_if_t < std::is_same_v<typename Data::template elem_t<SearchColumn>, std::string> ,int > = 0>
	inline auto Search(const std::string& search_for) const
	{
		if constexpr (std::is_convertible_v<typename Data::template elem_t<SearchColumn>, std::string>)
		{
			return mDataRef.like_index<SearchColumn>(ConstructRegex(search_for));
		}
		return std::vector<size_t>{};
	}

	std::regex ConstructRegex(const std::string& text) const
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
	Data& mDataRef;
};